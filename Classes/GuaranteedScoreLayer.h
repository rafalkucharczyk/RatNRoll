#ifndef __GUARANTEEDSCORE_LAYER_H__
#define __GUARANTEEDSCORE_LAYER_H__

#include <cocos2d.h>

#include "BasicWhiteLayer.h"
#include "Menu/MenuHelper.h"
#include "Menu/DigitsPanel.h"

class GuaranteedScoreLayer : public BasicWhiteLayer
{
  public:
    GuaranteedScoreLayer(int currentScore, int currentScoreThreshold, int nextScoreThreshold);
    static GuaranteedScoreLayer *create(int currentScore, int currentScoreThreshold,
                                        int nextScoreThreshold);

    bool init() override;

  private:
    void itemClicked(int itemIndex);

    void selfCleanUp();

    MenuHelper menuHelper;

    int currentScore;
    int currentScoreThreshold;
    int nextScoreThreshold;
};

#endif /* __GUARANTEEDSCORE_LAYER_H__ */