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

Vec2 BackgroundLayer::getRandomStartPoint()
{
    if (rand_minus1_1() > 0) {
        return Vec2(0, rand_0_1() * visibleSize.y);
    }

    return Vec2(rand_0_1() * visibleSize.x, visibleSize.y);
}

Vec2 BackgroundLayer::getTargetPoint(const Vec2 &startPoint)
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

        if (p != startPoint && p != Vec2::ZERO && p.x >= 0 && p.x <= visibleSize.x && p.y >= 0 &&
            p.y <= visibleSize.y) {
            return p;
        }
    }

    return Vec2::ZERO;
}

bool BackgroundLayer::init()
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
