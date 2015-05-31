#include "HelloWorldScene.h"

#include <vector>

USING_NS_CC;
using namespace std;

class BackgroundLayer : public Layer
{
  public:
    BackgroundLayer(const std::string &itemFileName, const std::string &backgroundFileName)
        : totalTime(0), itemsSpeed(0.2), itemsDirection(1, -2),
          visibleSize(Director::getInstance()->getVisibleSize()), desiredItemsCount(20),
          itemFileName(itemFileName), backgroundFileName(backgroundFileName)
    {
    }

    virtual bool init()
    {
        if (!Layer::init()) {
            return false;
        }

        auto sprite = Sprite::create(backgroundFileName);
        sprite->setPosition(visibleSize / 2);
        sprite->setScale(1.2);
        addChild(sprite);

        scheduleUpdate();

        const int steps = 5;
        runAction(Repeat::create(Sequence::create(CallFunc::create([this]() {
                                                      this->addBackgroundItems(desiredItemsCount /
                                                                               steps);
                                                  }),
                                                  DelayTime::create(1), nullptr),
                                 steps));

        return true;
    };

    void addBackgroundItems(int count)
    {
        for (int i = 0; i < count; i++) {
            insertBackgroundItem();
        }
    }

    void insertBackgroundItem()
    {
        Vec2 initialPosition = getRandomStartPoint();
        Vec2 targetPosition = getTargetPoint(initialPosition);

        float lengthRatio = (targetPosition - initialPosition).length() / visibleSize.length();

        auto sprite = Sprite::create(itemFileName);
        sprite->setPosition(initialPosition);
        sprite->setScale(1 + 0.5 * rand_0_1());
        sprite->runAction(RepeatForever::create(RotateBy::create(2 + 2 * rand_0_1(), 360)));

        float duration = lengthRatio * 1 / itemsSpeed;
        duration += rand_minus1_1() * duration * 0.1;

        items.push_back(BackgroundItem(sprite, totalTime, targetPosition, duration));

        addChild(sprite);
    }

    Vec2 getRandomStartPoint()
    {
        if (rand_minus1_1() > 0) {
            return Vec2(0, rand_0_1() * visibleSize.y);
        }

        return Vec2(rand_0_1() * visibleSize.x, visibleSize.y);
    }

    Vec2 getTargetPoint(const Vec2 &startPoint)
    {
        vector<Vec2> vertices = {
            Vec2(0, visibleSize.y),             // top-left
            Vec2(visibleSize.x, visibleSize.y), // top-right
            Vec2(visibleSize.x, 0),             // bottom-right
            Vec2(0, 0),                         // bottom-left
            Vec2(0, visibleSize.y)              // top-left
        };

        for (auto i = vertices.begin(); i != vertices.end() - 1; ++i) {
            Vec2 p = Vec2::getIntersectPoint(*i, *(i + 1), startPoint, startPoint + itemsDirection);

            if (p != startPoint && p != Vec2::ZERO && p.x >= 0 && p.x <= visibleSize.x &&
                p.y >= 0 && p.y <= visibleSize.y) {
                return p;
            }
        }

        return Vec2::ZERO;
    }

    virtual void update(float time)
    {
        totalTime += time;

        int toBeAddedCount = 0;

        for (auto i = items.begin(); i != items.end();) {
            if (totalTime < i->startTime + i->duration) {
                i->sprite->setPosition(i->getPositionForTime(totalTime));
                ++i;
            } else {
                removeChild(i->sprite);
                i = items.erase(i);
                toBeAddedCount++;
            }
        }

        addBackgroundItems(toBeAddedCount);
    }

    // speed: 0.0-1.0, 1.0 -> 1 sec to go along diagonal of the screen.
    void setSpeed(float newSpeed) { itemsSpeed = newSpeed; }

    static BackgroundLayer *create(const std::string &itemFileName,
                                   const std::string &backgroundFileName)
    {
        BackgroundLayer *ret = new (std::nothrow) BackgroundLayer(itemFileName, backgroundFileName);
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return NULL;
    }

  private:
    struct BackgroundItem {
        BackgroundItem(Sprite *sprite, float startTime, Vec2 point, float duration)
            : sprite(sprite), startTime(startTime), targetPoint(point), duration(duration)
        {
            initialPoint = sprite->getPosition();
        }

        Vec2 getPositionForTime(float time)
        {
            return initialPoint +
                   (targetPoint - initialPoint) * ((time - startTime) * (1 / duration));
        }

        Sprite *sprite;
        float startTime, duration;
        Vec2 initialPoint, targetPoint;
    };

  private:
    float totalTime;

    float itemsSpeed;
    Vec2 itemsDirection;

    Vec2 visibleSize;

    int desiredItemsCount;

    std::string itemFileName, backgroundFileName;

    vector<BackgroundItem> items;
};

Scene *HelloWorld::createScene()
{
    srand(time(NULL));

    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();
    auto backgroundLayer = BackgroundLayer::create("cheese01.png", "background01.png");

    // add layer as a child to scene
    scene->addChild(backgroundLayer);
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if (!BasicRUBELayer::init()) {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    float scale = 1. / this->getScale();

    auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    closeItem->setScale(scale);
    closeItem->setPosition(
        Vec2(scale * (origin.x + visibleSize.width / 2 - closeItem->getContentSize().width / 2),
             scale * (origin.y + closeItem->getContentSize().height / 2)));

    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    return true;
}

void HelloWorld::menuCloseCallback(Ref *pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    MessageBox("You pressed the close button. Windows Store Apps do not "
               "implement a close button.",
               "Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

// Override this in subclasses to specify which .json file to load
std::string HelloWorld::getFilename() { return "test.json"; }

// Override this in subclasses to set the inital view position
Point HelloWorld::initialWorldOffset()
{
    // This function should return the location in pixels to place
    // the (0,0) point of the physics world. The screen position
    // will be relative to the bottom left corner of the screen.

    // place (0,0) of physics world at center of bottom edge of screen
    Size s = Director::getInstance()->getWinSize();
    return Vec2(s.width / 2, 0);
}

// Override this in subclasses to set the inital view scale
float HelloWorld::initialWorldScale()
{
    // This method should return the number of pixels for one physics unit.
    // When creating the scene in RUBE I can see that the jointTypes scene
    // is about 8 units high, so I want the height of the view to be about
    // 10 units, which for iPhone in landscape (480x320) we would return 32.
    // But for an iPad in landscape (1024x768) we would return 76.8, so to
    // handle the general case, we can make the return value depend on the
    // current screen height.

    Size s = Director::getInstance()->getWinSize();
    return s.height / 10; // screen will be 10 physics units high
}
