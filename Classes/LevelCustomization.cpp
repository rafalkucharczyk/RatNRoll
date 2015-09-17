#include "LevelCustomization.h"

#include "AchievementTracker.h"

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
