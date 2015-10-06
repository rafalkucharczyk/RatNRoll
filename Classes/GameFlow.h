#ifndef __GAMEFLOW_H__
#define __GAMEFLOW_H__

#include <cocos2d.h>

#include "SonarCocosHelper/GameCenterPlayerScore.h"

class LevelCustomization;
class LevelLayer;
class InAppPurchaseHelper;

class GameFlow
{
  public:
    GameFlow();
    static GameFlow &getInstance();

  public:
    cocos2d::Scene *createInitialScene();

    void pauseGame();

    bool iapPurchaseCompleted() const;

  private:
    void switchToInitialScene();

    void handleInitialSceneMenu(int itemIndex);

    void switchToLevelScene(int levelNumber);
    void switchToLevelSceneWithScores(int levelNumber,
                                      SonarCocosHelper::GameCenterPlayersScores scores);

    void switchToPostLevelScene(int score);

    void switchToSettingsScene();

#ifdef COCOS2D_DEBUG
    void switchToTestScene();
#endif // COCOS2D_DEBUG

    void resumeGame();

  private:
    void loginToGameCenter();

    void startChallenge(SonarCocosHelper::GameCenterPlayerScore score);

    std::string getLeaderboardName(int levelNumber);

  private:
    cocos2d::Scene *createSceneObject();
    LevelCustomization *getLevelCustomization(int levelNumber) const;
    int updateBestScore(int levelNumber, int score);

    LevelLayer &getCurrentLevelLayer();

    int currentLevelNumber;

  private:
    std::string getSocialShareMessage(int score);

  private:
    std::shared_ptr<InAppPurchaseHelper> iapHelper;

    static const std::string iapProductId;

  private:
    static GameFlow *instance;

    const static int pauseLayerTag = 123;
    const static int backgroundLayerTag = 124;
    const static int noLevelNumber = -1;
};

#endif // __GAMEFLOW_H__
