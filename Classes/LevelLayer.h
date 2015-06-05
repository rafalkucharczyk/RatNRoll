#ifndef __LEVEL_LAYER_H__
#define __LEVEL_LAYER_H__

#include "cocos2d.h"

#include "RUBELayer.h"

class LevelLayer : public RUBELayer
{
  public:
    LevelLayer();
    static cocos2d::Scene *createScene();

    virtual bool init();

    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform,
                      uint32_t transformUpdated);

    CREATE_FUNC(LevelLayer);

    virtual std::string getFilename();
    virtual cocos2d::Point initialWorldOffset();
    virtual float initialWorldScale();

    virtual void afterLoadProcessing(b2dJson *json);

    virtual void update(float dt);

    virtual bool drawDebugData() { return true; }

  private:
    b2Body *ratBody, *earthBody;

    b2Vec2 propellerForce;

    float totalTime;
};

#endif // __LEVEL_SCENE_H__
