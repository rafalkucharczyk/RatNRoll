#ifndef __POST_TUTORIAL_LAYER_H__
#define __POST_TUTORIAL_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"

class PostTutorialLayer : public cocos2d::Layer
{
  public:
    PostTutorialLayer();
    virtual bool init();

    CREATE_FUNC(PostTutorialLayer);

    void setRestartLevelCallback(std::function<void()> callback);
    void setGotoMainMenuCallback(std::function<void()> callback);

  private:
    void menuItemClicked(int itemIndex);

    std::function<void()> restartLevelCallback;
    std::function<void()> gotoMainMenuCallback;

  private:
    void addAnimation();

    MenuHelper menuHelper;
};

#endif // __POST_TUTORIAL_LAYER_H__
