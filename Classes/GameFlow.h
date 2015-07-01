#ifndef __GAMEFLOW_H__
#define __GAMEFLOW_H__

#include <cocos2d.h>

class LevelCustomization;

class GameFlow
{
  public:
    static GameFlow &getInstance();

  public:
    cocos2d::Scene *createInitialScene();

    void switchToInitialScene();

    void switchToLevelScene(int levelNumber);

    void switchToPostLevelScene(int score);

  private:
    LevelCustomization *getLevelCustomization(int levelNumber) const;
    int updateBestScore(int levelNumber, int score);

    int currentLevelNumber;

  private:
    static GameFlow *instance;
};

#endif // __GAMEFLOW_H__
