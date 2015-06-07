#ifndef __LEVEL_LAYER_H__
#define __LEVEL_LAYER_H__

#include "cocos2d.h"

#include "RUBELayer.h"

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

    void setGameFinishedCallback(std::function<void()> callback)
    {
        gameFinishedCallback = callback;
    }

  private:
    b2Body *ratBody, *earthBody, *cageBody;

    b2Vec2 propellerForce;

    float totalTime;

    std::shared_ptr<LevelContactListener> contactListener;

    friend class LevelContactListener;

    std::function<void()> gameFinishedCallback;
};

#endif // __LEVEL_SCENE_H__
