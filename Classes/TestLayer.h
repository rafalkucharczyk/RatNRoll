#ifndef __TEST_LAYER_H__
#define __TEST_LAYER_H__

#ifdef COCOS2D_DEBUG

#include "cocos2d.h"

class TestLayer : public cocos2d::LayerColor
{
  public:
    TestLayer();
    virtual bool init();

    CREATE_FUNC(TestLayer);

  private:
    std::string getEyeAnimation();
};

#endif // COCOS2D_DEBUG

#endif // __TEST_LAYER_H__
