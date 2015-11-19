#ifndef __FACEBOOK_LIKE_LAYER_H__
#define __FACEBOOK_LIKE_LAYER_H__

#include "cocos2d.h"

class FacebookLikeLayer : public cocos2d::Layer
{
  public:
    FacebookLikeLayer(const cocos2d::Vec2 &buttonPosition, float buttonSize);
    virtual bool init();

    static FacebookLikeLayer *create(const cocos2d::Vec2 &buttonPosition, float buttonSize);

    void setLikingCompletedCallback(std::function<void()> callback)
    {
        likingCompletedCallback = callback;
    }

  private:
    std::function<void()> likingCompletedCallback;
    cocos2d::Vec2 buttonPosition;
    float buttonSize;
};

#endif // __FACEBOOK_LIKE_LAYER_H__
