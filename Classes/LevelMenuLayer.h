#ifndef __LEVELMENU_LAYER_H__
#define __LEVELMENU_LAYER_H__

#include "cocos2d.h"

class LevelMenuLayer : public cocos2d::Layer
{
  public:
    virtual bool init();

    void menuCloseCallback(cocos2d::Ref *sender);

    CREATE_FUNC(LevelMenuLayer);
};

#endif // __LEVELMENU_LAYER_H__
