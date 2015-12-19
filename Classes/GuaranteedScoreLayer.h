#ifndef __GUARANTEEDSCORE_LAYER_H__
#define __GUARANTEEDSCORE_LAYER_H__

#include <cocos2d.h>

#include "Menu/MenuHelper.h"
#include "Menu/DigitsPanel.h"

class GuaranteedScoreLayer : public cocos2d::LayerColor
{
  public:
    GuaranteedScoreLayer(int currentScore, int currentScoreThreshold, int nextScoreThreshold);
    static GuaranteedScoreLayer *create(int currentScore, int currentScoreThreshold,
                                        int nextScoreThreshold);

    bool init() override;

  private:
    void itemClicked(int itemIndex);

    void selfCleanUp();

    std::function<void()> closeCallback;

    MenuHelper menuHelper;

    int currentScore;
    int currentScoreThreshold;
    int nextScoreThreshold;
};

#endif /* __GUARANTEEDSCORE_LAYER_H__ */
