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
    : canDropNewItem(true), currentItemIndex(0), cogwheelHelper(new CogwheelHelper())
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

Level01::Level01(bool frenzyEnabled, bool shieldEnabled)
    : LevelBase(frenzyEnabled, shieldEnabled), cogwheelHelper(new CogwheelHelper())
{
}

void Level01::additionalAfterLoadProcessing(b2dJson *json)
{
    cogwheelHelper->additionalAfterLoadProcessing(json);
}

void Level01::customPhysicsStep(b2Body *earthBody) { cogwheelHelper->customPhysicsStep(earthBody); }

cocos2d::FiniteTimeAction *
LevelTutorial::levelStartedCallback(std::shared_ptr<LevelLayerProxy> levelLayerProxy,
                                    AchievementTracker &achievementTracker)
{
    achievementTracker.tutorialEntered();

    return spawnTutorialBalloon(TutorialBalloonLayer::BalloonType::CONTROLS, levelLayerProxy);
}

void LevelTutorial::notifyAchivementTracker(AchievementTracker &achievementTracker)
{
    if (shownBalloons.size() ==
        static_cast<size_t>(TutorialBalloonLayer::BalloonType::MAX_TYPE_COUNT)) {
        achievementTracker.tutorialCompleted();
    }
}
