#ifndef __GAMEFLOW_H__
#define __GAMEFLOW_H__

#include <cocos2d.h>

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

    bool adsVisible() const;

  private:
    struct Challenge {
        Challenge() : score(0) {}
        Challenge(const std::string &playerName, int score) : playerName(playerName), score(score)
        {
        }

        std::string playerName;
        int score;
    };

  private:
    void switchToInitialScene();

    void handleInitialSceneMenu(int itemIndex);

    void switchToLevelScene(int levelNumber);
    void switchToLevelSceneWithChallenge(int levelNumber, Challenge challenge);

    void switchToPostLevelScene(int score);

    void switchToSettingsScene();

    void resumeGame();

    void startChallenge(std::string playerName, int score, std::string leaderboardId);

  private:
    cocos2d::Scene *createSceneObject();
    LevelCustomization *getLevelCustomization(int levelNumber) const;
    int updateBestScore(int levelNumber, int score);

    LevelLayer &getCurrentLevelLayer();

    int currentLevelNumber;

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
