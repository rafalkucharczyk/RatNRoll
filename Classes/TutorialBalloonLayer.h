#ifndef __TUTORIALBALLOON_LAYER_H__
#define __TUTORIALBALLOON_LAYER_H__

#include <cocos2d.h>

#include "MenuHelper.h"

class TutorialBalloonLayer : public cocos2d::Layer
{
  public:
    enum class BalloonType { CONTROLS, SPEEDUP, SLOWDOWN, HOVER, HALVE };

    TutorialBalloonLayer(BalloonType balloonType, std::function<void()> closeCallback);
    static TutorialBalloonLayer *create(BalloonType balloonType,
                                        std::function<void()> closeCallback);

    bool init() override;

    void onExit() override;

  private:
    void addBalloonSprite();
    std::string getAnimationName();

    void itemClicked(int itemIndex);

    void selfCleanUp();

    BalloonType balloonType;
    std::function<void()> closeCallback;

    MenuHelper menuHelper;
};

#endif /* __TUTORIALBALLOON_LAYER_H__ */
