#ifndef __INITIAL_LAYER_H__
#define __INITIAL_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"

class InitialLayer : public cocos2d::Layer
{
  public:
    InitialLayer();
    virtual bool init();

    CREATE_FUNC(InitialLayer);

    void setMenuItemClickedCallback(std::function<void(int)> callback)
    {
        menuItemClickedCallback = callback;
    }

    void itemClicked(int itemIndex)
    {
        if (menuItemClickedCallback) {
            menuItemClickedCallback(itemIndex);
        }
    }

  private:
    MenuHelper menuHelper;
    std::function<void(int)> menuItemClickedCallback;
};

#endif // __INITIAL_LAYER_H__
