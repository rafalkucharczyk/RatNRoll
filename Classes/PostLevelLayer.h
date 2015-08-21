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

    void setRestartLevelCallback(std::function<void()> callback);
    void setGotoMainMenuCallback(std::function<void()> callback);
    void setShareOnFacebookCallback(std::function<void()> callback);
    void setShareOnTwitterCallback(std::function<void()> callback);

    void displayCurrentScore(int score);
    void displayBestScore(int score);

  private:
    void menuItemClicked(int itemIndex);

    std::function<void()> restartLevelCallback;
    std::function<void()> gotoMainMenuCallback;
    std::function<void()> shareOnFacebookCallback;
    std::function<void()> shareOnTwitterCallback;

    MenuHelper menuHelper;

    cocos2d::Label *currentScoreLabel;
    cocos2d::Label *bestScoreLabel;
};

#endif // __POSTLEVEL_LAYER_H__
