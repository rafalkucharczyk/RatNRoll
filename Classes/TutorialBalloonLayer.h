#ifndef __TUTORIALBALLOON_LAYER_H__
#define __TUTORIALBALLOON_LAYER_H__

#include <cocos2d.h>

#include <spine/spine-cocos2dx.h>

#include "MenuHelper.h"
#include "Menu/DigitsPanel.h"

class TutorialBalloonLayer : public cocos2d::LayerColor
{
  public:
    enum class BalloonType { CONTROLS, SPEEDUP, SLOWDOWN, HOVER, HALVE, MAX_TYPE_COUNT };

    TutorialBalloonLayer(BalloonType balloonType, std::function<void()> closeCallback);
    static TutorialBalloonLayer *create(BalloonType balloonType,
                                        std::function<void()> closeCallback);

    bool init() override;

    void onExit() override;

  private:
    void addBalloonSprite();
    std::string getAnimationName() const;
    cocos2d::Vec2 getAnimationInitPos() const;

    float getAnimationDuration(const std::string &animationName) const;

    void itemClicked(int itemIndex);

    void selfCleanUp();

    BalloonType balloonType;
    std::function<void()> closeCallback;

    MenuHelper menuHelper;

    spine::SkeletonAnimation *animationNode;
    DigitsPanel *digitsPanel;
};

#endif /* __TUTORIALBALLOON_LAYER_H__ */
