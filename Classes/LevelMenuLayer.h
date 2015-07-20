#ifndef __LEVELMENU_LAYER_H__
#define __LEVELMENU_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"

class LevelMenuLayer : public cocos2d::Layer
{
  public:
    LevelMenuLayer();
    virtual bool init();

    CREATE_FUNC(LevelMenuLayer);

    void setGamePausedCallback(std::function<void()> callback) { gamePausedCallback = callback; }

    void itemClicked(int itemIndex)
    {
        if (gamePausedCallback) {
            gamePausedCallback();
        }
    }

  private:
    MenuHelper menuHelper;
    std::function<void()> gamePausedCallback;
};

#endif // __LEVELMENU_LAYER_H__
