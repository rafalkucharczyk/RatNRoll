#ifndef __LEVEL_LAYER_H__
#define __LEVEL_LAYER_H__

#include "cocos2d.h"

#include "RUBELayer.h"
#include "b2dJson.h"

class LevelContactListener;

class LevelLayer : public RUBELayer
{
  public:
    LevelLayer();

    virtual bool init();

    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform,
                      uint32_t transformUpdated);

    CREATE_FUNC(LevelLayer);

    virtual std::string getFilename();
    virtual cocos2d::Point initialWorldOffset();
    virtual float initialWorldScale();

    virtual void afterLoadProcessing(b2dJson *json);

    virtual void update(float dt);

    virtual bool drawDebugData() { return false; }

    void setGameFinishedCallback(std::function<void(int)> callback)
    {
        gameFinishedCallback = callback;
    }

  public:
    enum ItemType { SPEEDUP = 0, SLOWDOWN, ITEM_TYPE_MAX };

  private:
    void doPhysicsCalculationStep();
    void updateScore();
    void dropItem(float t);
    void removeOutstandingItems();
    void ratAteItem(ItemType itemType);

    std::string itemTypeToImageName(ItemType itemType) const;

    void initScoreLabel();

  private:
    b2Body *ratBody, *earthBody, *cageBody;

    typedef std::list<b2Body *> BodiesList;
    BodiesList itemsBodies;
    BodiesList itemsToRemove; // items to be removed are gathered in ContactListener
                              // bodies can not be simply removed there

    Json::Value itemJsons[ITEM_TYPE_MAX]; // keeps Json::Value for every item parsed from RUBE file
                                          // needed for duplication
    b2dJson jsonParser;

    const float ratMinSpeed;
    const float ratMaxSpeed;
    const float ratSpeedStep;
    float ratTargetSpeed;

  private:
    b2RevoluteJoint *earthRevoluteJoint;
    int score;
    float previousRevoluteJointAngle;

    cocos2d::Label *scoreLabel;

  private:
    b2Vec2 propellerForce;
    float totalTime;

    std::shared_ptr<LevelContactListener> contactListener;

    friend class LevelContactListener;

    std::function<void(int)> gameFinishedCallback;
};

#endif // __LEVEL_SCENE_H__
