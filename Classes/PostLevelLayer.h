#ifndef __POSTLEVEL_LAYER_H__
#define __POSTLEVEL_LAYER_H__

#include "cocos2d.h"

class PostLevelLayer : public cocos2d::Layer
{
  public:
    static cocos2d::Scene *createScene();

    virtual bool init();

    CREATE_FUNC(PostLevelLayer);

    void setRestartLevelCallback(std::function<void()> callback)
    {
        restartLevelCallback = callback;
    }

  private:
    void restartCallback(cocos2d::Ref *sender);

    std::function<void()> restartLevelCallback;
};

#endif // __POSTLEVEL_LAYER_H__
