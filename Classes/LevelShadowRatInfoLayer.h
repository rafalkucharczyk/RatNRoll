#ifndef __LEVELSHADOWRATINFO_LAYER_H__
#define __LEVELSHADOWRATINFO_LAYER_H__

#include "cocos2d.h"

class LevelShadowRatInfoLayer : public cocos2d::Layer
{
  public:
    LevelShadowRatInfoLayer();

    CREATE_FUNC(LevelShadowRatInfoLayer);

    void showShadowRatInfo(const std::string &playerName, int score, int state);

  private:
    cocos2d::Node *infoNode;
};

#endif // __LEVELSHADOWRATINFO_LAYER_H__
