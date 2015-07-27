#ifndef __LEVEL_LAYER_H__
#define __LEVEL_LAYER_H__

#include "cocos2d.h"

#include "RUBELayer.h"
#include "b2dJson.h"
#include <spine/spine-cocos2dx.h>

#include "LevelCustomization.h"

class LevelContactListener;

class LevelLayer : public RUBELayer
{
  public:
    LevelLayer(LevelCustomization *customization);

    virtual bool init();

    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform,
                      uint32_t transformUpdated);

    static LevelLayer *create(LevelCustomization *customization);

    virtual std::string getFilename();
    virtual cocos2d::Point initialWorldOffset();
    virtual float initialWorldScale();

    virtual void afterLoadProcessing(b2dJson *json);

    virtual void update(float dt);
    void pauseLevel() { paused = true; }
    void resumeLevel() { paused = false; }

    bool setCustomImagePositionsFromPhysicsBodies(const RUBEImageInfo *imageInfo,
                                                  cocos2d::Point &position, float &angle);

    virtual bool drawDebugData() { return false; }

    bool isFixtureTouchable(b2Fixture *fixture) override;

    void setGameFinishedCallback(std::function<void(int)> callback)
    {
        gameFinishedCallback = callback;
    }

    void setBackgroundSpeedFunction(std::function<void(int)> function)
    {
        backgroundSpeedFunction = function;
    }

    static const int proxyOverlayLayer = 1111;

  private:
    void runCustomActionOnStart();
    void startDroppingItems();
    void stopDroppingItems();
    void doPhysicsCalculationStep();
    void updateScore();

    void dropItem(float t);
    b2Body *duplicateItem(LevelCustomization::ItemType itemType);
    void runShowItemAction(b2Body *body);
    friend class LevelLayerProxyImpl;
    void runDropItemAction(b2Body *body);

    void removeOutstandingItems();
    void ratAteItem(LevelCustomization::ItemType itemType);

    void speedUpItemEaten();
    void slowDownItemEaten();
    void hoverItemEaten();
    void halveItemEaten();

    std::string itemTypeToImageName(LevelCustomization::ItemType itemType) const;

    cocos2d::Label *initScoreLabel(int score);

    spine::SkeletonAnimation *getRatAnimation();

  private:
    std::unique_ptr<LevelCustomization> levelCustomization;
    b2Body *ratBody, *earthBody, *cageBody;

    typedef std::list<b2Body *> BodiesList;
    BodiesList itemsBodies;
    BodiesList itemsToRemove; // items to be removed are gathered in ContactListener
                              // bodies can not be simply removed there

    // keeps Json::Value for every item parsed from RUBE file, needed for duplication
    Json::Value itemJsons[LevelCustomization::ITEM_TYPE_MAX];

    b2dJson jsonParser;

    float ratSpeed;

    std::function<void(int)> backgroundSpeedFunction;

    bool applyCustomGravity;

  private:
    b2RevoluteJoint *earthRevoluteJoint;
    int gameScore;
    float previousRevoluteJointAngle;

    cocos2d::Label *scoreLabel;

  private:
    float totalTime;
    bool paused;

    std::shared_ptr<LevelContactListener> contactListener;

    friend class LevelContactListener;

    std::function<void(int)> gameFinishedCallback;

  public:
    static const std::string name;
};

#endif // __LEVEL_SCENE_H__
