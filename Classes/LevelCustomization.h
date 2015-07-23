#ifndef __LEVEL_CUSTOMIZATION_H__
#define __LEVEL_CUSTOMIZATION_H__

#include <list>
#include <string>

#include <Box2D/Common/b2Math.h>

#include <base/ccRandom.h>

class LevelCustomization
{
  public:
    virtual ~LevelCustomization() {}

  public:
    enum ItemType { SPEEDUP = 0, SLOWDOWN, HOVER, HALVE, ITEM_TYPE_MAX };

  public:
    virtual std::string getRubeJsonFileName() const = 0;

    virtual float getItemDropInterval() = 0; // in seconds

    virtual float getRatSpeedMin() = 0;
    virtual float getRatSpeedMax() = 0;
    virtual float getRatSpeedStep() = 0;

    // return ITEM_TYPE_MAX to skip dropping item
    virtual ItemType getDropItemType(float currentRatSpeed) = 0;

    // initial position of dropped item (in Box2D's world coordinates)
    virtual b2Vec2 getDropItemSpot() = 0;

    // called by LevelLayer each time item is removed from world
    virtual void itemRemovedCallback(ItemType itemType){};

  protected:
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

    std::string getRubeJsonFileName() const { return "level_tutorial.json"; }

    float getItemDropInterval() { return 2.5; }

    float getRatSpeedMin() { return 0.5; }
    float getRatSpeedMax() { return 2.0; }
    float getRatSpeedStep() { return 0.2; }

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

    b2Vec2 getDropItemSpot() { return b2Vec2(0, 9); }

    virtual void itemRemovedCallback(ItemType itemType) { canDropNewItem = true; }

  private:
    int droppedItemsCount;
    bool canDropNewItem;
};

class Level01 : public LevelCustomization
{
  public:
    std::string getRubeJsonFileName() const { return "level_01.json"; }

    float getItemDropInterval() { return 3.0; }

    float getRatSpeedMin() { return 0.5; }
    float getRatSpeedMax() { return 5.0; }
    float getRatSpeedStep() { return 0.5; }

    ItemType getDropItemType(float currentRatSpeed)
    {
        ItemType itemType = static_cast<ItemType>(rand() % ITEM_TYPE_MAX);

        return normalizeDropItemType(itemType, currentRatSpeed);
    }

    b2Vec2 getDropItemSpot() { return b2Vec2(cocos2d::rand_minus1_1(), 9); }
};

#endif // __LEVEL_LOGIC_H__
