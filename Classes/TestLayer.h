#ifndef __TEST_LAYER_H__
#define __TEST_LAYER_H__

#ifdef COCOS2D_DEBUG

#include "cocos2d.h"
#include "Menu/MenuHelper.h"

class TestLayer : public cocos2d::LayerColor
{
  public:
    TestLayer();
    virtual bool init();

    CREATE_FUNC(TestLayer);

    void setGotoMainMenuCallback(std::function<void()> callback)
    {
        gotoMainMenuCallback = callback;
    }

  private:
    MenuHelper menuHelper;
    std::string getEyeAnimation();

    std::function<void()> gotoMainMenuCallback;

    void menuItemClicked(int itemIndex);
};

#endif // COCOS2D_DEBUG

#endif // __TEST_LAYER_H__
