#ifndef __ABOUT_LAYER_H__
#define __ABOUT_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"

class AboutLayer : public cocos2d::Layer
{
  public:
    AboutLayer();
    virtual bool init();

    CREATE_FUNC(AboutLayer);

    void menuItemClicked(int itemIndex);

    void setGotoMainMenuCallback(std::function<void()> callback)
    {
        gotoMainMenuCallback = callback;
    }

    void setResetGameStateCallback(std::function<void()> callback)
    {
        resetGameStateCallback = callback;
    }

    void setGotoTestLayerCallback(std::function<void()> callback)
    {
        gotoTestLayerCallback = callback;
    }

  private:
    MenuHelper menuHelper;

    std::function<void()> gotoMainMenuCallback;
    std::function<void()> resetGameStateCallback;
    std::function<void()> gotoTestLayerCallback;
};

#endif // __ABOUT_LAYER_H__
