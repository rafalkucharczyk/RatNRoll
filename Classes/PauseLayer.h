#ifndef __PAUSE_LAYER_H__
#define __PAUSE_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"

class PauseLayer : public cocos2d::LayerColor
{
  public:
    PauseLayer();
    virtual bool init();

    CREATE_FUNC(PauseLayer);

    void setGameResumedCallback(std::function<void()> callback) { gameResumedCallback = callback; }

    void setGameQuitCallback(std::function<void()> callback) { gameQuitCallback = callback; }

    void itemClicked(int itemIndex);

  private:
    MenuHelper menuHelper;
    std::function<void()> gameResumedCallback;
    std::function<void()> gameQuitCallback;
};

#endif // __PAUSE_LAYER_H__
