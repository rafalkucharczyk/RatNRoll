#ifndef __LEVEL_BLOCKER_LAYER_H__
#define __LEVEL_BLOCKER_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"
#include "LevelLayer.h"

class LevelLayer;

class LevelBlockerLayer : public cocos2d::LayerColor
{
  public:
    LevelBlockerLayer(LevelLayer &levelLayer);
    virtual bool init();

    static LevelBlockerLayer *create(LevelLayer &levelLayer);

    void setGotoMenuCallback(std::function<void()> callback) { gameGotoMenuCallback = callback; }

    void unblock();

    void onEnterTransitionDidFinish();

  private:
    void itemClicked(int itemIndex);

    MenuHelper menuHelper;
    std::function<void()> gameGotoMenuCallback;

    LevelLayer &levelLayer;
};

#endif // __LEVEL_BLOCKER_LAYER_H__
