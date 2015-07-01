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
    enum ItemType { SPEEDUP = 0, SLOWDOWN, ITEM_TYPE_MAX };

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
        // repeatedly drop 3 items of each type
        ItemType itemType = droppedItemsCount++ % 6 <= 2 ? SPEEDUP : SLOWDOWN;

        if (currentRatSpeed == getRatSpeedMax()) {
            itemType = SLOWDOWN;
        }

        if (currentRatSpeed == getRatSpeedMin()) {
            itemType = SPEEDUP;
        }

        return itemType;
    }

    b2Vec2 getDropItemSpot() { return b2Vec2(0, 10); }

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

        if (currentRatSpeed == getRatSpeedMax()) {
            itemType = SLOWDOWN;
        }

        if (currentRatSpeed == getRatSpeedMin()) {
            itemType = SPEEDUP;
        }

        return itemType;
    }

    b2Vec2 getDropItemSpot() { return b2Vec2(cocos2d::rand_minus1_1() * 1.5, 10); }
};

#endif // __LEVEL_LOGIC_H__
