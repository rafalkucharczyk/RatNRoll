#include "BackgroundLayer.h"

#include <vector>

USING_NS_CC;
using namespace std;

BackgroundLayer::BackgroundLayer(const std::string &itemFileName,
                                 const std::string &backgroundFileName)
    : totalTime(0), itemsSpeed(0.2), itemsDirection(1, -2),
      visibleSize(Director::getInstance()->getVisibleSize()), desiredItemsCount(20),
      itemFileName(itemFileName), backgroundFileName(backgroundFileName)
{
}

void BackgroundLayer::addBackgroundItems(int count)
{
    for (int i = 0; i < count; i++) {
        insertBackgroundItem();
    }
}

void BackgroundLayer::insertBackgroundItem()
{
    auto sprite = Sprite::create(itemFileName);

    Vec2 initialPosition = getRandomStartPoint(sprite->getContentSize());
    Vec2 targetPosition = getTargetPoint(initialPosition, sprite->getContentSize());

    float lengthRatio = (targetPosition - initialPosition).length() / visibleSize.length();

    sprite->setPosition(initialPosition);
    sprite->setScale(1 + 0.5 * rand_0_1());
    sprite->runAction(RepeatForever::create(RotateBy::create(2 + 2 * rand_0_1(), 360)));

    float duration = lengthRatio * 1 / itemsSpeed;
    duration += rand_minus1_1() * duration * 0.1;

    items.push_back(BackgroundItem(sprite, totalTime, targetPosition, duration));

    addChild(sprite);
}

Vec2 BackgroundLayer::getRandomStartPoint(const Size &spriteSize)
{
    if (rand_minus1_1() > 0) {
        return Vec2(-spriteSize.width, rand_0_1() * visibleSize.y); // left edge
    }

    return Vec2(rand_0_1() * visibleSize.x, visibleSize.y - spriteSize.height); // top edge
}

Vec2 BackgroundLayer::getTargetPoint(const Vec2 &startPoint, const cocos2d::Size &spriteSize)
{
    const float X = spriteSize.width;
    const float Y = spriteSize.height;

    vector<Vec2> vertices = {
        Vec2(visibleSize.x + X, visibleSize.y + Y), // top-right
        Vec2(visibleSize.x + X, -Y),                // bottom-right
        Vec2(-X, -Y),                               // bottom-left
        Vec2(-X, visibleSize.y + Y),                // top-left
        Vec2(visibleSize.x + X, visibleSize.y + Y)  // top-right
    };

    for (auto i = vertices.begin(); i != vertices.end() - 1; ++i) {
        Vec2 p = Vec2::getIntersectPoint(*i, *(i + 1), startPoint, startPoint + itemsDirection);

        if (p != startPoint && p != Vec2::ZERO && p.x >= -X && p.x <= visibleSize.x + X &&
            p.y >= -Y && p.y <= visibleSize.y + Y) {
            return p;
        }
    }

    return Vec2::ZERO;
}

BackgroundLayer::BackgroundItem::BackgroundItem(cocos2d::Sprite *sprite, float startTime,
                                                cocos2d::Vec2 point, float duration)
    : sprite(sprite), startTime(startTime), targetPoint(point), duration(duration)
{
    initialPoint = sprite->getPosition();
}

cocos2d::Vec2 BackgroundLayer::BackgroundItem::getPositionForTime(float time)
{
    return initialPoint + (targetPoint - initialPoint) * ((time - startTime) * (1 / duration));
}

bool BackgroundLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    auto sprite = Sprite::create(backgroundFileName);
    sprite->setPosition(visibleSize / 2);
    sprite->setScale(visibleSize.x / sprite->getContentSize().width,
                     visibleSize.y / sprite->getContentSize().height);
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

void BackgroundLayer::update(float time)
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
void BackgroundLayer::setSpeed(float newSpeed) { itemsSpeed = newSpeed; }

BackgroundLayer *BackgroundLayer::create(const std::string &itemFileName,
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
