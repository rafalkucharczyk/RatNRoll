#ifndef __BACKGROUND_LAYER_H__
#define __BACKGROUND_LAYER_H__

#include "cocos2d.h"

#include <string>
#include <vector>

struct BackgroundItemInitialState {
    std::string fileName;
    cocos2d::Vec2 position;
    float scale;
    float rotation;
};

typedef std::vector<BackgroundItemInitialState> BackgroundItemsInitialState;

class BackgroundLayer : public cocos2d::Layer
{
  public:
    BackgroundLayer(const std::vector<std::string> &itemFileNames,
                    const std::string &backgroundFileName);

    virtual bool init();

    static void
    setBackgroundItemsInitialState(const BackgroundItemsInitialState &backgroundItemsInitialState);

    virtual void update(float time);

    // e.g: +1 increase current speed by delta, -2 decrease current speed by two deltas
    void setSpeed(int deltasCount);

    static BackgroundLayer *create(const std::vector<std::string> &itemFileNames,
                                   const std::string &backgroundFileName);

  private:
    struct BackgroundItem {
        BackgroundItem(cocos2d::Sprite *sprite, cocos2d::Vec2 targetPoint);

        cocos2d::Vec2 getPositionForTime(float time);

        cocos2d::Sprite *sprite;
        float startTime, duration;
        cocos2d::Vec2 initialPoint, targetPoint;
    };

  private:
    typedef std::function<cocos2d::Vec2(const cocos2d::Size &)> RandomPositionFunction;

    typedef std::function<BackgroundItemInitialState()> BackgroundItemInitialStateProvider;

    BackgroundItemInitialState
    randomizeBackgroundItemInitialState(RandomPositionFunction positionFunction);

    void addBackgroundItems(int count,
                            BackgroundItemInitialStateProvider backgroundItemInitialStateProvider);
    void insertBackgroundItem(const BackgroundItemInitialState &backgroundItemInitialState);
    void setBackgroundItemParams(cocos2d::Vec2 targetPosition, cocos2d::Vec2 initialPosition,
                                 BackgroundItem &item);
    cocos2d::Vec2 getRandomStartPointOnEdge(const cocos2d::Size &spriteSize);
    cocos2d::Vec2 getRandomStartPointEntireScreen(const cocos2d::Size &spriteSize);
    cocos2d::Vec2 getTargetPoint(const cocos2d::Vec2 &startPoint, const cocos2d::Size &spriteSize);

  private:
    const float minItemsSpeed;
    const float maxItemsSpeed;
    const float deltaItemsSpeed;

    static BackgroundItemsInitialState backgroundItemsInitialState;

    float totalTime;

    float itemsSpeed;
    cocos2d::Vec2 itemsDirection;

    cocos2d::Vec2 visibleSize;

    int desiredItemsCount;

    std::vector<std::string> itemFileNames;
    std::string backgroundFileName;

    std::vector<BackgroundItem> items;
};

#endif // __BACKGROUND_LAYER_H__
