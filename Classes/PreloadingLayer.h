#ifndef __PRELOADING_LAYER_H__
#define __PRELOADING_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"

class PreloadingLayer : public cocos2d::LayerColor
{
  public:
    PreloadingLayer();
    virtual bool init();

    CREATE_FUNC(PreloadingLayer);

    void setPreloadingCompletedCallback(std::function<void()> callback)
    {
        preloadingCompletedCallback = callback;
    }

  private:
    std::function<void()> preloadingCompletedCallback;
};

#endif // __PRELOADING_LAYER_H__
