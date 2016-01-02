#include "GuaranteedScoreLayer.h"
#include "ToString.h"

#include "Menu/MenuLabel.h"

#include "PostLevelLayer.h"

USING_NS_CC;

GuaranteedScoreLayer::GuaranteedScoreLayer(int currentScore, int currentScoreThreshold,
                                           int nextScoreThreshold)
    : currentScore(currentScore), currentScoreThreshold(currentScoreThreshold),
      nextScoreThreshold(nextScoreThreshold),
      menuHelper({{{0.5, 0.23}, 0.1, "confirm", {MenuHelper::removeTransparency()}}},
                 std::bind(&GuaranteedScoreLayer::itemClicked, this, std::placeholders::_1))
{
}

GuaranteedScoreLayer *GuaranteedScoreLayer::create(int currentScore, int currentScoreThreshold,
                                                   int nextScoreThreshold)
{
    GuaranteedScoreLayer *ret = new (std::nothrow)
        GuaranteedScoreLayer(currentScore, currentScoreThreshold, nextScoreThreshold);
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return NULL;
}

bool GuaranteedScoreLayer::init()
{
    if (!BasicWhiteLayer::init()) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    EventListenerTouchOneByOne *listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [&](Touch *touch, Event *event) { return true; };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,
                                                                                          this);

    auto menuLabelTop =
        MenuLabel::create("YOU'VE SCORED MORE THAN\n" + std::to_string(currentScoreThreshold) +
                              " POINTS!\n\n"
                              "FROM NOW ON,\n"
                              "THIS WILL BE YOUR\n"
                              "INITIAL SCORE IN THIS LEVEL.",
                          {0.5, 0.55}, 0.03);
    menuLabelTop->setCascadeOpacityEnabled(true);
    menuLabelTop->setOpacity(0);

    addChild(menuLabelTop);
    menuLabelTop->runAction(FadeIn::create(1.0));

    if (nextScoreThreshold != 0) {
        auto menuLabelBottom = MenuLabel::create(
            "NEXT THRESHOLD IS\n" + std::to_string(nextScoreThreshold) + " POINTS.", {0.5, 0.4},
            0.03);
        menuLabelBottom->setCascadeOpacityEnabled(true);
        menuLabelBottom->setOpacity(0);

        addChild(menuLabelBottom);
        menuLabelBottom->runAction(FadeIn::create(1.0));
    }

    PostLevelLayer::addDigitsPanel(*this, {0.5, 0.8}, "YOUR SCORE", currentScore);

    return true;
}

void GuaranteedScoreLayer::itemClicked(int itemIndex) { removeFromParentAndCleanup(true); }
