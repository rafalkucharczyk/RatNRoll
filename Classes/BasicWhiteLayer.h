#ifndef __BASICWHITE_LAYER_H__
#define __BASICWHITE_LAYER_H__

#include <2d/CCLayer.h>

class BasicWhiteLayer : public cocos2d::LayerColor
{
  public:
    virtual bool init();

    CREATE_FUNC(BasicWhiteLayer);
};

#endif // __BASICWHITE_LAYER_H__
