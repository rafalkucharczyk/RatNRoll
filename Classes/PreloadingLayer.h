#ifndef __PRELOADING_LAYER_H__
#define __PRELOADING_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"

class PreloadingLayer : public cocos2d::Layer
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
    void preload(float t);

    void insertBackgroundItem();
    std::vector<std::string> filesToPreload;

    std::function<void()> preloadingCompletedCallback;

    int currentItemsCount;
    static const int maxItemsCount = 10;
};

#endif // __PRELOADING_LAYER_H__
