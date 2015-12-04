#include "AchievementTracker.h"
#include "SonarCocosHelper/SonarFrameworks.h"
#include "PermanentStorage.h"
#include "LevelLayer.h"

#include <chrono>

USING_NS_CC;

#define ACHIEVEMENT(Name)                                                                          \
    struct Name {                                                                                  \
        static constexpr const char *name = #Name;                                                 \
    };

namespace Achievements
{
ACHIEVEMENT(SUPER_SIZE_ME)    // 100 burgers
ACHIEVEMENT(AS_FAT_AS_CAN_BE) // 100 triple burgers
ACHIEVEMENT(CARDIAC)          // 1000 burgers or triple burgers
ACHIEVEMENT(JUNKIE)           // 100 vials
ACHIEVEMENT(ALCHEMIST)        // 1000 vials
ACHIEVEMENT(FREQUENT_FLYER)   // 100 geese
ACHIEVEMENT(ANGEL)            // 1000 geese
ACHIEVEMENT(TIME_WASTER)      // 15sec in credits
ACHIEVEMENT(DILIGENT_STUDENT) // tutorial completed
ACHIEVEMENT(RETARD)           // tutorial entered 5 times
ACHIEVEMENT(JACK_THE_REAPER)  // 100 skulls
ACHIEVEMENT(DEVIL)            // 1000 skulls
ACHIEVEMENT(JACKPOT)          // three items of the same kind caught one after another
ACHIEVEMENT(KLEPTOMANIAC)     // all kind of items caught
ACHIEVEMENT(CHEESEIONAIRE)    // 100 cheese frenzy
ACHIEVEMENT(SUICIDE)          // 10 game over
ACHIEVEMENT(ROAD_RUNNER)      // run with maximum speed for the first time
ACHIEVEMENT(GET_A_LIFE)       // sharing via twitter or facebook
ACHIEVEMENT(PRO_RUNNER)       // reach fall out point with the highest speed and recover
ACHIEVEMENT(
    WE_DO_NOT_KNOW_HOW_TO_NAME_THIS_ACHIEVEMENT) // pause/resume game 3 times within 5 seconds
ACHIEVEMENT(I_BELIEVE_I_CAN_FLY)                 // 1st goose
ACHIEVEMENT(WRIGHT_BROTHERS)                     // first fall after hovering
ACHIEVEMENT(SHIFT_MANAGER)                       // score 999999 points in the Cheese Factory
ACHIEVEMENT(GRAVEDIGGER)                         // score 999999 points in the Graveyard
ACHIEVEMENT(MAGICIAN)                            // score 999999 points in the Magic Hall
};

namespace
{
const static int creditsEnteredActionTag = 1026;
const static int hoverEatenActionTag = 1027;

// uses system time instead of actions because actions are suspended when game is paused
std::chrono::time_point<std::chrono::system_clock> gamePausedStartTime =
    std::chrono::system_clock::from_time_t(0);
}

AchievementTracker::AchievementTracker()
    : state(PermanentStorage::getInstance().getAchievementTrackerState()),
      unlocked(PermanentStorage::getInstance().getUnlockedAchievements())
{
}

void AchievementTracker::itemCaught(LevelCustomization::ItemType itemType)
{
    state.itemsCount[itemType]++;

    if (itemType == LevelCustomization::SLOWDOWN) {
        unlock(Achievements::SUPER_SIZE_ME::name,
               state.itemsCount[LevelCustomization::SLOWDOWN] / 100.);
    }

    if (itemType == LevelCustomization::BREAK) {
        unlock(Achievements::AS_FAT_AS_CAN_BE::name,
               state.itemsCount[LevelCustomization::BREAK] / 100.);
    }

    if (itemType == LevelCustomization::SLOWDOWN || itemType == LevelCustomization::BREAK) {
        unlock(Achievements::CARDIAC::name, (state.itemsCount[LevelCustomization::BREAK] +
                                             state.itemsCount[LevelCustomization::SLOWDOWN]) /
                                                1000.);
    }

    if (itemType == LevelCustomization::SPEEDUP) {
        unlock(Achievements::JUNKIE::name, state.itemsCount[LevelCustomization::SPEEDUP] / 100.);
        unlock(Achievements::ALCHEMIST::name,
               state.itemsCount[LevelCustomization::SPEEDUP] / 1000.);
    }

    if (itemType == LevelCustomization::HOVER) {
        if (state.itemsCount[LevelCustomization::HOVER] == 1) {
            unlock(Achievements::I_BELIEVE_I_CAN_FLY::name);
        }

        unlock(Achievements::FREQUENT_FLYER::name,
               state.itemsCount[LevelCustomization::HOVER] / 100.);
        unlock(Achievements::ANGEL::name, state.itemsCount[LevelCustomization::HOVER] / 1000.);
    }

    if (itemType == LevelCustomization::HALVE) {
        unlock(Achievements::JACK_THE_REAPER::name,
               state.itemsCount[LevelCustomization::HALVE] / 100.);
        unlock(Achievements::DEVIL::name, state.itemsCount[LevelCustomization::HALVE] / 1000.);
    }

    if (itemType == LevelCustomization::FRENZY) {
        unlock(Achievements::CHEESEIONAIRE::name,
               state.itemsCount[LevelCustomization::FRENZY] / 100.);
    }

    // JACKPOT

    if (state.sequence.first == itemType) {
        state.sequence.second++;
    } else {
        state.sequence = std::make_pair(itemType, 1);
    }

    if (state.sequence.second == 3) {
        unlock(Achievements::JACKPOT::name);
    }

    // KLEPTOMANIAC

    if (std::all_of(std::begin(state.itemsCount), std::end(state.itemsCount),
                    [](int count) { return count != 0; })) {
        unlock(Achievements::KLEPTOMANIAC::name);
    }

    // WRIGHT_BROTHERS

    if (itemType == LevelCustomization::HOVER) {
        Action *action = DelayTime::create(2 * LevelLayer::hoverDuration);
        action->setTag(hoverEatenActionTag);
        runAction(action);
    }

    PermanentStorage::getInstance().setAchievementTrackerState(state);
}

void AchievementTracker::creditsEntered()
{
    Action *action = Sequence::create(
        DelayTime::create(15),
        CallFunc::create([this]() { unlock(Achievements::TIME_WASTER::name); }), nullptr);
    action->setTag(creditsEnteredActionTag);
    runAction(action);
}

void AchievementTracker::creditsExited() { stopAction(getActionByTag(creditsEnteredActionTag)); }

void AchievementTracker::tutorialEntered()
{
    if (++state.tutorialEnteredCount == 5) {
        unlock(Achievements::RETARD::name);
    }

    PermanentStorage::getInstance().setAchievementTrackerState(state);
}

void AchievementTracker::tutorialCompleted() { unlock(Achievements::DILIGENT_STUDENT::name); }

void AchievementTracker::gameEnded()
{
    if (++state.gameOverCount <= 10) {
        unlock(Achievements::SUICIDE::name, state.gameOverCount / 10.);
    }

    Action *action = getActionByTag(hoverEatenActionTag);

    if (action && !action->isDone()) {
        unlock(Achievements::WRIGHT_BROTHERS::name);
    }

    PermanentStorage::getInstance().setAchievementTrackerState(state);
}

void AchievementTracker::maxSpeedReached() { unlock(Achievements::ROAD_RUNNER::name); }

void AchievementTracker::scoreShared() { unlock(Achievements::GET_A_LIFE::name); }

void AchievementTracker::proRunner() { unlock(Achievements::PRO_RUNNER::name); }

void AchievementTracker::gamePaused()
{
    if (gamePausedStartTime == std::chrono::system_clock::from_time_t(0)) {
        gamePausedStartTime = std::chrono::system_clock::now();
    }
}

void AchievementTracker::gameResumed()
{
    std::chrono::duration<double> elapsed_seconds =
        std::chrono::system_clock::now() - gamePausedStartTime;

    if (elapsed_seconds.count() <= 5.0) {
        if (++state.resumeCount == 3) {
            unlock(Achievements::WE_DO_NOT_KNOW_HOW_TO_NAME_THIS_ACHIEVEMENT::name);
        }
    } else {
        gamePausedStartTime = std::chrono::system_clock::from_time_t(0);
        state.resumeCount = 0;
    }
}

void AchievementTracker::levelCompleted(int levelNumber)
{
    if (levelNumber == 1) {
        unlock(Achievements::SHIFT_MANAGER::name);
    }

    if (levelNumber == 2) {
        unlock(Achievements::GRAVEDIGGER::name);
    }

    if (levelNumber == 3) {
        unlock(Achievements::MAGICIAN::name);
    }
}

void AchievementTracker::unlock(const std::string &achievementName, float progress)
{
    if (unlocked.find(achievementName) != unlocked.end()) {
        return;
    }

    if (progress >= 1.0) {
        unlocked[achievementName] = 1;
    }

    SonarCocosHelper::GameCenter::unlockAchievement(achievementName,
                                                    100 * std::min(1.0f, std::max(0.0f, progress)));

    PermanentStorage::getInstance().setUnlockedAchievements(unlocked);
}
