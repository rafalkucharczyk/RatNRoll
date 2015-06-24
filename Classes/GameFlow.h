#ifndef __GAMEFLOW_H__
#define __GAMEFLOW_H__

#include <cocos2d.h>

class GameFlow
{
  public:
    static cocos2d::Scene *createLevelScene();

    static void switchToLevelScene();

    static void switchToPostLevelScene(int score);

  private:
    static int updateBestScore(int score);
};

#endif // __GAMEFLOW_H__
