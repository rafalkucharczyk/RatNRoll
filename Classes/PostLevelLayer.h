#ifndef __POSTLEVEL_LAYER_H__
#define __POSTLEVEL_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"

class PostLevelLayer : public cocos2d::Layer
{
  public:
    PostLevelLayer();
    virtual bool init();

    CREATE_FUNC(PostLevelLayer);

    void setRestartLevelCallback(std::function<void()> callback)
    {
        restartLevelCallback = callback;
    }

    void setGotoMainMenuCallback(std::function<void()> callback)
    {
        gotoMainMenuCallback = callback;
    }

    void setShareOnFacebookCallback(std::function<void()> callback)
    {
        shareOnFacebookCallback = callback;
    }

    void displayCurrentScore(int score);
    void displayBestScore(int score);

  private:
    void menuItemClicked(int itemIndex);

    std::function<void()> restartLevelCallback;
    std::function<void()> gotoMainMenuCallback;
    std::function<void()> shareOnFacebookCallback;

    MenuHelper menuHelper;

    cocos2d::Label *currentScoreLabel;
    cocos2d::Label *bestScoreLabel;
};

#endif // __POSTLEVEL_LAYER_H__
