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

    void setSettingsCallback(std::function<void()> callback) { settingsCallback = callback; }

    void setPlayCallback(std::function<void()> callback) { playCallback = callback; }

    void setGameCenterCallbacks(std::function<bool()> isSignedInCallback,
                                std::function<void(bool signedIn)> actionCallback)
    {
        gameCenterIsSignedInCallback = isSignedInCallback;
        gameCenterActionCallback = actionCallback;
    }

  private:
    void itemClicked(int itemIndex);

    MenuHelper menuHelper;

    std::function<void()> settingsCallback;
    std::function<void()> playCallback;

    std::function<bool()> gameCenterIsSignedInCallback;
    std::function<void(bool signedIn)> gameCenterActionCallback;
};

#endif // __INITIAL_LAYER_H__
