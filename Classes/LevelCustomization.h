#ifndef __LEVEL_CUSTOMIZATION_H__
#define __LEVEL_CUSTOMIZATION_H__

#include <list>
#include <string>

#include <Box2D/Common/b2Math.h>

#include <cocos2d.h>
#include "TutorialBalloonLayer.h"

// Functions available for LevelCustomization to interact with LevelLayer
class LevelLayerProxy
{
  public:
    virtual void pause() = 0;
    virtual void resume() = 0;

    virtual void addOverlayingLayer(cocos2d::Layer *layer) = 0;
};

class LevelCustomization
{
  public:
    virtual ~LevelCustomization() {}

  public:
    enum ItemType { SPEEDUP = 0, SLOWDOWN, HOVER, HALVE, BREAK, ITEM_TYPE_MAX };

  public:
    virtual std::string getRubeJsonFileName() const = 0;

    virtual float getItemDropInterval() = 0; // in seconds

    virtual float getRatSpeedMin() const = 0;
    virtual float getRatSpeedMax() const = 0;
    virtual float getRatSpeedStep() const = 0;
    virtual float getRatSpeedInitial() const { return getRatSpeedMin(); }

    // return ITEM_TYPE_MAX to skip dropping item
    virtual ItemType getDropItemType(float currentRatSpeed) = 0;

    // initial position of dropped item (in Box2D's world coordinates)
    virtual b2Vec2 getDropItemSpot(const b2Vec2 &ratPosition) = 0;

    // called by LevelLayer once, when level was loaded and started
    virtual cocos2d::FiniteTimeAction *
    levelStartedCallback(std::shared_ptr<LevelLayerProxy> levelLayerProxy)
    {
        return nullptr;
    };

    // called by LevelLayer each time item is removed from the world
    virtual void itemRemovedCallback(ItemType itemType){};

    // called by LevelLayer each time item is added to the world
    virtual cocos2d::FiniteTimeAction *
    itemAddedCallback(std::shared_ptr<LevelLayerProxy> levelLayerProxy, ItemType itemType)
    {
        return nullptr;
    };

  protected:
    bool isItemPositive(ItemType itemType) { return itemType == HOVER || itemType == SPEEDUP; }

    ItemType normalizeDropItemType(ItemType itemType, float currentRatSpeed)
    {
        if (currentRatSpeed == getRatSpeedMax() && itemType == SPEEDUP) {
            return SLOWDOWN;
        }

        if (currentRatSpeed == getRatSpeedMin() && itemType == SLOWDOWN) {
            return SPEEDUP;
        }

        return itemType;
    }
};

class LevelTutorial : public LevelCustomization
{
  public:
    LevelTutorial() : canDropNewItem(true) {}

    std::string getRubeJsonFileName() const { return "level_01.json"; }

    float getItemDropInterval() { return 2.5; }

    float getRatSpeedMin() const override { return 0.2; }
    float getRatSpeedMax() const override { return 3.2; }
    float getRatSpeedStep() const override { return 0.3; }
    float getRatSpeedInitial() const override { return getRatSpeedMin() + getRatSpeedStep(); }

    ItemType getDropItemType(float currentRatSpeed)
    {
        if (!canDropNewItem) {
            return ITEM_TYPE_MAX;
        }

        canDropNewItem = false;

        // repeatedly drop 1 item of each type
        ItemType itemType = ITEM_TYPE_MAX;

        if (droppedItemsCount % ITEM_TYPE_MAX == SPEEDUP) {
            itemType = SPEEDUP;
        } else if (droppedItemsCount % ITEM_TYPE_MAX == SLOWDOWN) {
            itemType = SLOWDOWN;
        } else if (droppedItemsCount % ITEM_TYPE_MAX == HOVER) {
            itemType = HOVER;
        } else {
            itemType = HALVE;
        }

        droppedItemsCount++;

        return normalizeDropItemType(itemType, currentRatSpeed);
    }

    b2Vec2 getDropItemSpot(const b2Vec2 &ratPosition) { return b2Vec2(0, 9); }

    cocos2d::FiniteTimeAction *
    levelStartedCallback(std::shared_ptr<LevelLayerProxy> levelLayerProxy) override
    {
        return spawnTutorialBalloon(TutorialBalloonLayer::BalloonType::CONTROLS, levelLayerProxy);
    }

    void itemRemovedCallback(ItemType itemType) override { canDropNewItem = true; }

    cocos2d::FiniteTimeAction *itemAddedCallback(std::shared_ptr<LevelLayerProxy> levelLayerProxy,
                                                 ItemType itemType) override
    {
        return spawnTutorialBalloon(itemTypeToBalloonType(itemType), levelLayerProxy);
    }

  private:
    cocos2d::FiniteTimeAction *
    spawnTutorialBalloon(TutorialBalloonLayer::BalloonType balloonType,
                         std::shared_ptr<LevelLayerProxy> levelLayerProxy)
    {
        USING_NS_CC;

        if (shownBalloons.find(balloonType) != shownBalloons.end()) {
            return nullptr;
        }

        shownBalloons.insert(balloonType);

        return Sequence::create(CallFunc::create([levelLayerProxy]() { levelLayerProxy->pause(); }),
                                CallFunc::create([=]() {
                                    levelLayerProxy->addOverlayingLayer(
                                        TutorialBalloonLayer::create(
                                            balloonType, [=]() { levelLayerProxy->resume(); }));
                                }),
                                nullptr);
    }

    TutorialBalloonLayer::BalloonType itemTypeToBalloonType(ItemType itemType)
    {
        if (itemType == SPEEDUP) {
            return TutorialBalloonLayer::BalloonType::SPEEDUP;
        } else if (itemType == SLOWDOWN) {
            return TutorialBalloonLayer::BalloonType::SLOWDOWN;
        } else if (itemType == HOVER) {
            return TutorialBalloonLayer::BalloonType::HOVER;
        } else if (itemType == HALVE) {
            return TutorialBalloonLayer::BalloonType::HALVE;
        }

        assert(false);
        return TutorialBalloonLayer::BalloonType::SPEEDUP;
    }

  private:
    int droppedItemsCount;
    bool canDropNewItem;

    std::set<TutorialBalloonLayer::BalloonType> shownBalloons;
};

class Level01 : public LevelCustomization
{
  public:
    Level01() : currentItemIndex(0), itemToDrop(ITEM_TYPE_MAX) {}

    std::string getRubeJsonFileName() const { return "level_01.json"; }

    float getItemDropInterval() { return 3.0; }

    float getRatSpeedMin() const override { return 0.2; }
    float getRatSpeedMax() const override { return 3.2; }
    float getRatSpeedStep() const override { return 0.3; }

    ItemType getDropItemType(float currentRatSpeed)
    {
        if (currentItemIndex == itemsSequence.size()) {
            currentItemIndex = 0;
        }

        ItemType itemType = static_cast<ItemType>(itemsSequence[currentItemIndex++]);

        itemToDrop = normalizeDropItemType(itemType, currentRatSpeed);

        return itemToDrop;
    }

    b2Vec2 getDropItemSpot(const b2Vec2 &ratPosition)
    {
        const float maxX = 1.2;
        const float maxXOffset = 0.3;
        float x =
            std::min(std::max(ratPosition.x + maxXOffset * cocos2d::rand_minus1_1(), -maxX), maxX);

        if (isItemPositive(itemToDrop)) {
            if (fabs(x) > maxXOffset) {
                x = -x;
            } else {
                x = ratPosition.x > 0 ? -maxX : maxX;
            }
        }

        return b2Vec2(x, 9);
    }

  private:
#include "LevelCustomization_itemsSequence.hxx"

    int currentItemIndex;
    ItemType itemToDrop;
};

#endif // __LEVEL_LOGIC_H__
