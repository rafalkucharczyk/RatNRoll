#ifndef __POSTLEVEL_LAYER_H__
#define __POSTLEVEL_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"
#include "Menu/DigitsPanel.h"

class PostLevelLayer : public cocos2d::Layer
{
  public:
    PostLevelLayer(int currentScore, int bestScore, int challengedScore,
                   const std::string &challengedUserName);
    virtual bool init();

    static PostLevelLayer *create(int currentScore, int bestScore, int challengedScore,
                                  const std::string &challengedUserName);

    void setRestartLevelCallback(std::function<void()> callback);
    void setGotoMainMenuCallback(std::function<void()> callback);

    void setShareOnFacebookCallback(std::function<void()> callback);
    void setShareOnTwitterCallback(std::function<void()> callback);

  private:
    void menuItemClicked(int itemIndex);

    std::function<void()> restartLevelCallback;
    std::function<void()> gotoMainMenuCallback;
    std::function<void()> shareOnFacebookCallback;
    std::function<void()> shareOnTwitterCallback;

  private:
    void addDigitsPanel(const cocos2d::Vec2 &position, const std::string &text, int score);
    void addAnimation(const std::string &name, const cocos2d::Vec2 &position);

    int currentScore;
    int bestScore;
    int challengedScore;
    std::string challengedUserName;

    MenuHelper menuHelper;
};

#endif // __POSTLEVEL_LAYER_H__
