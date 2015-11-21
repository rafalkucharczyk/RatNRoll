#include "LevelCustomization.h"

#include "AchievementTracker.h"

#include <Box2D/Dynamics/b2Body.h>
#include <b2dJson.h>

class CogwheelHelper
{
  public:
    CogwheelHelper() : cogwheelBody1(nullptr), cogwheelBody2(nullptr) {}

    void additionalAfterLoadProcessing(b2dJson *json)
    {
        cogwheelBody1 = json->getBodyByName("cogwheel1");
        assert(cogwheelBody1);
        cogwheelBody2 = json->getBodyByName("cogwheel2");
        assert(cogwheelBody2);
    }

    void customPhysicsStep(b2Body *earthBody)
    {
        cogwheelBody2->SetAngularVelocity(3 * earthBody->GetAngularVelocity());
        cogwheelBody1->SetAngularVelocity(-0.8 * cogwheelBody2->GetAngularVelocity());
    }

  private:
    b2Body *cogwheelBody1, *cogwheelBody2;
};

LevelTutorial::LevelTutorial()
    : canDropNewItem(true), currentItemIndex(0), cogwheelHelper(new CogwheelHelper()),
      tracker(nullptr)
{
}

void LevelTutorial::additionalAfterLoadProcessing(b2dJson *json)
{
    cogwheelHelper->additionalAfterLoadProcessing(json);
}

void LevelTutorial::customPhysicsStep(b2Body *earthBody)
{
    cogwheelHelper->customPhysicsStep(earthBody);
}

cocos2d::FiniteTimeAction *
LevelTutorial::levelStartedCallback(std::shared_ptr<LevelLayerProxy> levelLayerProxy,
                                    AchievementTracker &achievementTracker)
{
    tracker = &achievementTracker;

    achievementTracker.tutorialEntered();

    return spawnTutorialBalloon(TutorialBalloonLayer::BalloonType::WELCOME, levelLayerProxy);
}

cocos2d::FiniteTimeAction *
LevelTutorial::spawnTutorialBalloon(TutorialBalloonLayer::BalloonType balloonType,
                                    std::shared_ptr<LevelLayerProxy> levelLayerProxy)
{
    USING_NS_CC;

    auto i = shownBalloons.find(TutorialBalloonLayer::BalloonType::HALVE);

    if (i != shownBalloons.end() && i->second == 3) {
        balloonType = TutorialBalloonLayer::BalloonType::FINAL;
    }

    if (shownBalloons.find(balloonType) != shownBalloons.end()) {
        shownBalloons[balloonType]++;
        return nullptr;
    }

    shownBalloons.insert(std::make_pair(balloonType, 1));
    notifyTutorialCompletedAchievement();

    return Sequence::create(CallFunc::create([levelLayerProxy]() { levelLayerProxy->pause(); }),
                            CallFunc::create([=]() {
                                levelLayerProxy->addOverlayingLayer(TutorialBalloonLayer::create(
                                    balloonType, [=]() { levelLayerProxy->resume(); }));
                            }),
                            nullptr);
}

void LevelTutorial::notifyTutorialCompletedAchievement()
{
    if (shownBalloons.size() ==
        static_cast<size_t>(TutorialBalloonLayer::BalloonType::MAX_TYPE_COUNT)) {
        if (tracker) {
            tracker->tutorialCompleted();
        }
    }
}

Level01::Level01(bool frenzyEnabled, bool shieldEnabled)
    : LevelBase(frenzyEnabled, shieldEnabled), cogwheelHelper(new CogwheelHelper())
{
}

void Level01::additionalAfterLoadProcessing(b2dJson *json)
{
    cogwheelHelper->additionalAfterLoadProcessing(json);
}

void Level01::customPhysicsStep(b2Body *earthBody) { cogwheelHelper->customPhysicsStep(earthBody); }
