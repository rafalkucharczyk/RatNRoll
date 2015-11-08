#ifndef __GAMEFLOW_H__
#define __GAMEFLOW_H__

#include <cocos2d.h>

#include "SonarCocosHelper/GameCenterPlayerScore.h"
#include "SoundHelper.h"

class LevelCustomization;
class LevelLayer;
class InAppPurchaseHelper;
class AchievementTracker;

class GameFlow
{
  public:
    GameFlow();
    static GameFlow &getInstance();

  public:
    cocos2d::Scene *createInitialScene();

    void pauseGame();

    bool iapPurchaseCompleted() const;

    SoundSettings getSoundSettings() const;

  private:
    void setSoundSettings(const SoundSettings &settings);

  private:
    void switchToInitialScene();

    void handleInitialSceneMenu(int itemIndex);

    void switchToLevelSelectionScene();
    void switchToLevelScene(int levelNumber);
    void switchToLevelSceneWithScores(int levelNumber,
                                      SonarCocosHelper::GameCenterPlayersScores scores);

    void switchToPostLevelScene(int score);

    void switchToSettingsScene();

    void switchToAboutScene();

#ifdef COCOS2D_DEBUG
    void switchToTestScene();
#endif // COCOS2D_DEBUG

    void resumeGame();

  private:
    void loginToGameCenter();

    void startChallenge(SonarCocosHelper::GameCenterPlayerScore score);

    std::string getLeaderboardName(int levelNumber);

  private:
    cocos2d::Scene *createSceneObject(const std::string &bgPlaneName = "bg_plane01",
                                      const std::list<std::string> &bgItemNames = {"bg_item01"});
    LevelCustomization *getLevelCustomization(int levelNumber) const;
    int updateBestScore(int levelNumber, int score);

    LevelLayer &getCurrentLevelLayer();

    int currentLevelNumber;

  private:
    AchievementTracker &addAchievementTracker(cocos2d::Node &parent) const;
    AchievementTracker &getAchievementTracker() const;

  private:
    std::string getSocialShareMessage(int score);

  private:
    std::shared_ptr<InAppPurchaseHelper> iapHelper;

    static const std::string iapProductId;

  private:
    static GameFlow *instance;

    const static int pauseLayerTag = 123;
    const static int backgroundLayerTag = 124;
    const static int achievementTrackerTag = 125;
    const static int noLevelNumber = -1;
};

#endif // __GAMEFLOW_H__
