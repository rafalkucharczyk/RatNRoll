#ifndef __GAME_COMPLETED_LAYER_H__
#define __GAME_COMPLETED_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"

class GameCompletedLayer : public cocos2d::LayerColor
{
  public:
    GameCompletedLayer(int levelNumber);
    virtual bool init();

    static GameCompletedLayer *create(int levelNumber);

    void setCompletionConifrmedCallback(std::function<void()> callback)
    {
        completionConifrmedCallback = callback;
    }

    void itemClicked(int itemIndex);

  private:
    MenuHelper menuHelper;
    std::function<void()> completionConifrmedCallback;
    int levelNumber;
};

#endif // __GAME_COMPLETED_LAYER_H__
