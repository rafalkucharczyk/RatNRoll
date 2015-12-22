#ifndef __ABOUT_LAYER_H__
#define __ABOUT_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"

class AboutLayer : public cocos2d::LayerColor
{
  public:
    AboutLayer();
    virtual bool init();

    CREATE_FUNC(AboutLayer);

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
    float getTotalHeightOfAboutEntries();
    Layer *composeScrollableLayer();
    void addScrollView(Layer *scrollableLayer);

    void addBackgroundItems();

    void menuItemClicked(int itemIndex);

    MenuHelper menuHelper;

    std::function<void()> gotoMainMenuCallback;
    std::function<void()> resetGameStateCallback;
    std::function<void()> gotoTestLayerCallback;
};

#endif // __ABOUT_LAYER_H__
