#ifndef __UNLOCK_GRAVEYARD_LAYER_H__
#define __UNLOCK_GRAVEYARD_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"
#include "Facebook/FacebookLikeNode.h"

class UnlockGraveyardLayer : public cocos2d::Layer
{
  public:
    UnlockGraveyardLayer();
    virtual bool init();

    CREATE_FUNC(UnlockGraveyardLayer);

  private:
    FacebookLikeNode *facebookLikeNode;
};

#endif // __UNLOCK_GRAVEYARD_LAYER_H__
