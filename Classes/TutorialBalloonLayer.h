#ifndef __TUTORIALBALLOON_LAYER_H__
#define __TUTORIALBALLOON_LAYER_H__

#include <cocos2d.h>

#include <spine/spine-cocos2dx.h>

#include "MenuHelper.h"
#include "Menu/DigitsPanel.h"

struct TutorialScreen {
    std::string animationName;
    cocos2d::Vec2 animationPosition;
    std::string text;
    float textYPosition;
};

class TutorialBalloonLayer : public cocos2d::LayerColor
{
  public:
    enum class BalloonType { WELCOME, SPEEDUP, SLOWDOWN, HOVER, HALVE, FINAL, MAX_TYPE_COUNT };

    TutorialBalloonLayer(BalloonType balloonType, std::function<void()> closeCallback);
    static TutorialBalloonLayer *create(BalloonType balloonType,
                                        std::function<void()> closeCallback);

    bool init() override;

    void onExit() override;

  private:
    void addDigitsPanel();

    void createScreen(const TutorialScreen &screen);

    void itemClicked(int itemIndex);

    void selfCleanUp();

    std::function<void()> closeCallback;

    std::list<TutorialScreen> currentTutorialScreens;

    MenuHelper menuHelper;

    spine::SkeletonAnimation *animationNode;
    DigitsPanel *digitsPanel;
};

#endif /* __TUTORIALBALLOON_LAYER_H__ */
