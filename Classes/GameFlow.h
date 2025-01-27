#ifndef __GAMEFLOW_H__
#define __GAMEFLOW_H__

#include <cocos2d.h>

#include "SonarCocosHelper/GameCenterPlayerScore.h"
#include "SoundHelper.h"

#include "BackgroundLayer.h"

class LevelCustomization;
class LevelLayer;
class AchievementTracker;

class GameFlow
{
  public:
    GameFlow();
    static GameFlow &getInstance();

  public:
    cocos2d::Scene *createPreloadingScene();

    void pauseGame();
    void resumeGame();

    bool likingCompleted() const;
    bool iapPurchaseCompleted() const;

    SoundSettings getSoundSettings() const;

  private:
    void setSoundSettings(const SoundSettings &settings);

  private:
    void replaceScene(cocos2d::Scene *scene);

    void switchToInitialScene();

    void handleInitialSceneMenu(int itemIndex);

    void switchToLevelSelectionScene();
    void switchToLevelScene(int levelNumber);
    void switchToLevelSceneWithScores(int levelNumber,
                                      SonarCocosHelper::GameCenterPlayersScores scores);
    // true if level blocked
    bool blockLevel(cocos2d::Scene &scene, LevelLayer &levelLayer, int levelNumber,
                    const SonarCocosHelper::GameCenterPlayerScore &score);

    void switchToPostLevelScene(int score);
    void switchToPostTutorialScene();

    void switchToSettingsScene();

    void switchToAboutScene();

    void switchToTestScene();

    void overlayGameCompletedScene(LevelLayer *levelLayer);

    void overlayGameScoreThresholdScene(int gameScore, cocos2d::Scene *parentScene);

  private:
    void loginToGameCenter();

    void startChallenge(SonarCocosHelper::GameCenterPlayerScore score);

    std::string getLeaderboardName(int levelNumber);

    std::shared_ptr<SonarCocosHelper::GameCenterPlayerScore> currentChallenge;

    void clearCurrentChallenge();

  private:
    cocos2d::Scene *createSceneObject();
    void setCurrentBackgroundConfig(const BackgroundConfig &backgroundConfig);

    BackgroundConfig currentBackgroundConfig;

    LevelCustomization *getLevelCustomization(int levelNumber) const;
    int getBestScore(int levelNumber, int currentScore);
    int updateBestScore(int levelNumber, int currentScore);

    LevelLayer &getCurrentLevelLayer();

    int currentLevelNumber;

    bool isGamePlayActive() const;

  private:
    AchievementTracker &addAchievementTracker(cocos2d::Node &parent) const;
    AchievementTracker &getAchievementTracker() const;

  private:
    static const std::string iapProductId;

  private:
    static GameFlow *instance;

    const static int pauseLayerTag = 123;
    const static int backgroundLayerTag = 124;
    const static int achievementTrackerTag = 125;
    const static int levelBlockerTag = 126;
    const static int levelCompletedTag = 127;
    const static int noLevelNumber = -1;
};

#endif // __GAMEFLOW_H__
