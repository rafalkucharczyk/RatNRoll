#ifndef __LEVEL_SELECTION_LAYER_H__
#define __LEVEL_SELECTION_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"

class LevelSelectionLayer : public cocos2d::Layer
{
  public:
    LevelSelectionLayer(bool swingTutorialButton, bool swingCheeseFactoryButton);
    virtual bool init();

    static LevelSelectionLayer *create(bool swingTutorialButton, bool swingCheeseFactoryButton);

    void setLevelSelectedCallback(std::function<void(int)> callback)
    {
        levelSelectedCallback = callback;
    }

    void setBackButtonClickedCallback(std::function<void()> callback)
    {
        backButtonClickedCallback = callback;
    }

    void itemClicked(int itemIndex)
    {
        if (itemIndex <= 3) {
            if (levelSelectedCallback) {
                levelSelectedCallback(itemIndex);
            }
        } else {
            if (backButtonClickedCallback) {
                backButtonClickedCallback();
            }
        }
    }

  private:
    MenuHelper menuHelper;
    std::function<void(int)> levelSelectedCallback;
    std::function<void()> backButtonClickedCallback;
};

#endif // __LEVEL_SELECTION_LAYER_H__
