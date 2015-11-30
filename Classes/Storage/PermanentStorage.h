#ifndef __PERMANENTSTORAGE_H__
#define __PERMANENTSTORAGE_H__

#include <json.h>
#include "SoundHelper.h"
#include "AchievementTracker.h"
#include "TutorialBalloonLayer.h"

class PermanentStorage
{
  public:
    static PermanentStorage &getInstance();

    void setBestScore(int levelNumber, int score);
    int getBestScore(int levelNumber) const;

    void setPurchaseState(const std::string &productId, bool bought);
    bool getPurchaseState(const std::string &productId) const;

    void setLikingState(bool liked);
    bool getLikingState() const;

    void setSoundSettings(const SoundSettings &soundSettings);
    SoundSettings getSoundSettings() const;

    typedef std::map<std::string, int> CustomDataMap;

    void setUnlockedAchievements(const CustomDataMap &achievements);
    CustomDataMap getUnlockedAchievements() const;

    void setAchievementTrackerState(const AchievementTracker::State &state);
    AchievementTracker::State getAchievementTrackerState() const;

    void setTutorialEntered(bool entered);
    bool getTutorialEntered() const;

    void setTutorialStage(int ballonType);
    int getTutorialStage() const;

  private:
    PermanentStorage();

    void saveToKeychain(const std::string &keychainItemKey, bool value);
    bool readFromKeychain(const std::string &keychainItemKey) const;

    static PermanentStorage *instance;
};

#endif // __PERMANENTSTORAGE_H__
