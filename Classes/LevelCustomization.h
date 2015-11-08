#ifndef __LEVEL_CUSTOMIZATION_H__
#define __LEVEL_CUSTOMIZATION_H__

#include <list>
#include <string>

#include <Box2D/Common/b2Math.h>

#include <cocos2d.h>
#include "TutorialBalloonLayer.h"

class AchievementTracker;

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
    enum ItemType { SPEEDUP = 0, SLOWDOWN, HOVER, HALVE, BREAK, FRENZY, SHIELD, ITEM_TYPE_MAX };

  public:
    virtual std::string getRubeJsonFileName() const = 0;

    // Background plane for level searched in Resources/background (e.g bg_plane01)
    virtual std::string getBgPlaneName() const = 0;
    // List of background items for level searched in Resources/background (e.g bg_item01)
    virtual std::list<std::string> getBgItemNames() const = 0;

    virtual float getItemDropInterval(int gameScore) = 0; // in seconds

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
    levelStartedCallback(std::shared_ptr<LevelLayerProxy> levelLayerProxy,
                         AchievementTracker &achievementTracker)
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

    // called by LevelLayer when game is finished
    virtual void notifyAchivementTracker(AchievementTracker &achievementTracker){};

  protected:
    bool isItemPositive(ItemType itemType)
    {
        return itemType == HOVER || itemType == SPEEDUP || itemType == FRENZY || itemType == SHIELD;
    }

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
    LevelTutorial() : canDropNewItem(true), currentItemIndex(0) {}

    std::string getRubeJsonFileName() const override { return "level_01.json"; };

    std::string getBgPlaneName() const { return "bg_plane01"; }
    std::list<std::string> getBgItemNames() const { return {"bg_item01"}; }

    float getItemDropInterval(int gameScore) override { return 2.5; };

    float getRatSpeedMin() const override { return 0.2; }
    float getRatSpeedMax() const override { return 3.2; }
    float getRatSpeedStep() const override { return 0.3; }
    float getRatSpeedInitial() const override { return getRatSpeedMin() + getRatSpeedStep(); }

    ItemType getDropItemType(float currentRatSpeed) override
    {
        if (!canDropNewItem) {
            return ITEM_TYPE_MAX;
        }

        canDropNewItem = false;

        if (currentItemIndex == itemsSequence.size()) {
            currentItemIndex = 0;
        }

        ItemType itemType = static_cast<ItemType>(itemsSequence[currentItemIndex++]);

        return normalizeDropItemType(itemType, currentRatSpeed);
    }

    b2Vec2 getDropItemSpot(const b2Vec2 &ratPosition) override { return b2Vec2(0, 9); }

    cocos2d::FiniteTimeAction *
    levelStartedCallback(std::shared_ptr<LevelLayerProxy> levelLayerProxy,
                         AchievementTracker &achievementTracker);

    void itemRemovedCallback(ItemType itemType) override { canDropNewItem = true; }

    cocos2d::FiniteTimeAction *itemAddedCallback(std::shared_ptr<LevelLayerProxy> levelLayerProxy,
                                                 ItemType itemType) override
    {
        return spawnTutorialBalloon(itemTypeToBalloonType(itemType), levelLayerProxy);
    }

    void notifyAchivementTracker(AchievementTracker &achievementTracker);

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
    bool canDropNewItem;

    std::set<TutorialBalloonLayer::BalloonType> shownBalloons;

    int currentItemIndex;
    std::vector<int> itemsSequence = {0, 0, 1, 1, 2, 3};
};

class LevelBase : public LevelCustomization
{
  public:
    LevelBase(bool frenzyEnabled, bool shieldEnabled)
        : currentItemIndex(0), bonusItemInjected(false), itemToDrop(ITEM_TYPE_MAX),
          frenzyEnabled(frenzyEnabled), shieldEnabled(shieldEnabled)
    {
    }

    float getItemDropInterval(int gameScore) override
    {
        int minScore = 1000;
        int maxScore = 5000;

        float delta = float(gameScore - minScore) / (maxScore - minScore);

        delta = std::min(std::max(0.0f, delta), 1.0f);

        return 3.0 - 1.5 * delta * cocos2d::rand_0_1();
    }

    float getRatSpeedMin() const override { return 0.2; }
    float getRatSpeedMax() const override { return 4.2; }
    float getRatSpeedStep() const override { return 0.4; }

    ItemType getDropItemType(float currentRatSpeed) override
    {
#ifdef TEST_SEQUENCE
        itemsSequence = {FRENZY, SHIELD, HALVE};
#endif
        if (currentItemIndex == itemsSequence.size()) {
            currentItemIndex = 0;
        }
#ifdef TEST_SEQUENCE
        return static_cast<ItemType>(itemsSequence[currentItemIndex++]);
#endif
        ItemType bonusItemType = getBonusItem();

        if (bonusItemType != ITEM_TYPE_MAX) {
            return bonusItemType;
        }

        ItemType itemType = static_cast<ItemType>(itemsSequence[currentItemIndex++]);
        bonusItemInjected = false;

        itemToDrop = normalizeDropItemType(itemType, currentRatSpeed);

        return itemToDrop;
    }

    b2Vec2 getDropItemSpot(const b2Vec2 &ratPosition) override
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
    ItemType getBonusItem()
    {
        if (!frenzyEnabled && !shieldEnabled) {
            return ITEM_TYPE_MAX;
        }

        if (currentItemIndex != 0 && currentItemIndex % 10 == 0 && bonusItemInjected == false) {
            bonusItemInjected = true;

            std::vector<ItemType> bonusItems;

            if (frenzyEnabled) {
                bonusItems.push_back(FRENZY);
            }

            if (shieldEnabled) {
                bonusItems.push_back(SHIELD);
            }

            return bonusItems[cocos2d::random<int>(0, bonusItems.size() - 1)];
        }

        return ITEM_TYPE_MAX;
    }

  private:
#include "LevelCustomization_itemsSequence.hxx"

    int currentItemIndex;
    bool bonusItemInjected;
    ItemType itemToDrop;
    bool frenzyEnabled, shieldEnabled;
};

class Level01 : public LevelBase
{
  public:
    Level01(bool frenzyEnabled, bool shieldEnabled) : LevelBase(frenzyEnabled, shieldEnabled) {}
    std::string getRubeJsonFileName() const override { return "level_01.json"; }
    std::string getBgPlaneName() const { return "bg_plane01"; }
    std::list<std::string> getBgItemNames() const { return {"bg_item01"}; }
};

class Level02 : public LevelBase
{
  public:
    Level02(bool frenzyEnabled, bool shieldEnabled) : LevelBase(frenzyEnabled, shieldEnabled) {}
    std::string getRubeJsonFileName() const override { return "level_02.json"; }
    std::string getBgPlaneName() const { return "bg_plane02"; }
    std::list<std::string> getBgItemNames() const { return {"bg_item02"}; }
};

class Level03 : public LevelBase
{
  public:
    Level03(bool frenzyEnabled, bool shieldEnabled) : LevelBase(frenzyEnabled, shieldEnabled) {}
    std::string getRubeJsonFileName() const override { return "level_03.json"; }
    std::string getBgPlaneName() const { return "bg_plane03"; }
    std::list<std::string> getBgItemNames() const
    {
        return {"bg_item03", "bg_item03_1", "bg_item03_2", "bg_item03_3"};
    }
};

#endif // __LEVEL_LOGIC_H__
