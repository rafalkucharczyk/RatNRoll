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
    void switchToInitialScene();

    void handleInitialSceneMenu(int itemIndex);

    void switchToLevelScene(int levelNumber);

    void switchToPostLevelScene(int score);

    void switchToSettingsScene();

    void resumeGame();

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
