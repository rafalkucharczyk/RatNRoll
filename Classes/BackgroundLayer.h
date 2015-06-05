#ifndef __BACKGROUND_LAYER_H__
#define __BACKGROUND_LAYER_H__

#include "cocos2d.h"

#include <string>
#include <vector>

class BackgroundLayer : public cocos2d::Layer
{
  public:
    BackgroundLayer(const std::string &itemFileName, const std::string &backgroundFileName);

    virtual bool init();

    virtual void update(float time);

    void setSpeed(float newSpeed);

    static BackgroundLayer *create(const std::string &itemFileName,
                                   const std::string &backgroundFileName);

  private:
    void addBackgroundItems(int count);
    void insertBackgroundItem();
    cocos2d::Vec2 getRandomStartPoint(const cocos2d::Size &spriteSize);
    cocos2d::Vec2 getTargetPoint(const cocos2d::Vec2 &startPoint, const cocos2d::Size &spriteSize);

  private:
    struct BackgroundItem {
        BackgroundItem(cocos2d::Sprite *sprite, float startTime, cocos2d::Vec2 point,
                       float duration);

        cocos2d::Vec2 getPositionForTime(float time);

        cocos2d::Sprite *sprite;
        float startTime, duration;
        cocos2d::Vec2 initialPoint, targetPoint;
    };

  private:
    float totalTime;

    float itemsSpeed;
    cocos2d::Vec2 itemsDirection;

    cocos2d::Vec2 visibleSize;

    int desiredItemsCount;

    std::string itemFileName, backgroundFileName;

    std::vector<BackgroundItem> items;
};

#endif // __BACKGROUND_LAYER_H__
