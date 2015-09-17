#ifndef _ACHIEVEMENT_TRACKER_H_
#define _ACHIEVEMENT_TRACKER_H_

#include <cocos2d.h>

#include "LevelCustomization.h"

class AchievementTracker : public cocos2d::Node
{
  public:
    struct State {
        State()
            : tutorialEnteredCount(0),
              sequence(std::make_pair(LevelCustomization::ITEM_TYPE_MAX, 0)), gameOverCount(0),
              resumeCount(0)
        {
            std::fill(std::begin(itemsCount), std::end(itemsCount), 0);
        }

        int itemsCount[LevelCustomization::ITEM_TYPE_MAX];

        int tutorialEnteredCount;

        std::pair<LevelCustomization::ItemType, int> sequence;

        int gameOverCount;

        int resumeCount; // not persisted
    };

  public:
    AchievementTracker();

    CREATE_FUNC(AchievementTracker);

    void itemCaught(LevelCustomization::ItemType itemType);

    void creditsEntered();
    void creditsExited();

    void tutorialEntered();
    void tutorialCompleted();

    void gameEnded();

    void maxSpeedReached();

    void scoreShared();

    // this event must explicitly come from game engine
    // (impossible to calculate in AchievementTracker)
    void proRunner();

    void gamePaused();
    void gameResumed();

  private:
    void unlock(const std::string &achievementName);

    State state;
    std::map<std::string, int> unlocked;
};

#endif // _ACHIEVEMENT_TRACKER_H_