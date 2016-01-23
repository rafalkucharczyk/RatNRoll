#ifndef __GUARANTEEDSCORE_LAYER_H__
#define __GUARANTEEDSCORE_LAYER_H__

#include <cocos2d.h>

#include "BasicWhiteLayer.h"
#include "Menu/MenuHelper.h"
#include "Menu/DigitsPanel.h"
#include "Facebook/FacebookLikeNode.h"

class GuaranteedScoreLayer : public BasicWhiteLayer
{
  public:
    GuaranteedScoreLayer(int currentScore, int currentScoreThreshold, int nextScoreThreshold);
    static GuaranteedScoreLayer *create(int currentScore, int currentScoreThreshold,
                                        int nextScoreThreshold);

    bool init() override;

  private:
    void itemClicked(int itemIndex);

    void addAnimationNode();
    void addFacebookNode();

    MenuHelper menuHelper;

    int currentScore;
    int currentScoreThreshold;
    int nextScoreThreshold;

    FacebookLikeNode *facebookLikeNode;
};

#endif /* __GUARANTEEDSCORE_LAYER_H__ */
