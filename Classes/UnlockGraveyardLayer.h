#ifndef __UNLOCK_GRAVEYARD_LAYER_H__
#define __UNLOCK_GRAVEYARD_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"
#include "Facebook/FacebookLikeNode.h"

class UnlockGraveyardLayer : public cocos2d::Layer
{
  public:
    UnlockGraveyardLayer();
    virtual bool init(bool unlockingEnabled);

    static UnlockGraveyardLayer *create(bool unlockingEnabled);

    void setLikingCompletedCallback(std::function<void()> callback)
    {
        likingCompletedCallback = callback;
    }

  private:
    std::function<void()> likingCompletedCallback;

    void itemClicked(int itemIndex);

    MenuHelper menuHelper;
    FacebookLikeNode *facebookLikeNode;
};

#endif // __UNLOCK_GRAVEYARD_LAYER_H__
