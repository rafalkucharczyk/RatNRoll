#ifndef __GAMEFLOW_H__
#define __GAMEFLOW_H__

#include <cocos2d.h>

class LevelCustomization;

class GameFlow
{
  public:
    GameFlow();
    static GameFlow &getInstance();

  public:
    cocos2d::Scene *createInitialScene();

    void pauseGame();

  private:
    void switchToInitialScene();

    void handleInitialSceneMenu(int itemIndex);

    void switchToLevelScene(int levelNumber);

    void switchToPostLevelScene(int score);

    void resumeGame();

  private:
    LevelCustomization *getLevelCustomization(int levelNumber) const;
    int updateBestScore(int levelNumber, int score);

    int currentLevelNumber;

  private:
    static GameFlow *instance;

    const static int pauseLayerTag = 123;
    const static int noLevelNumber = -1;
};

#endif // __GAMEFLOW_H__
