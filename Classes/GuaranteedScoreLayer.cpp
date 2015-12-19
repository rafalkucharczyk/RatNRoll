#include "GuaranteedScoreLayer.h"

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
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 232))) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    EventListenerTouchOneByOne *listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [&](Touch *touch, Event *event) { return true; };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,
                                                                                          this);

    auto menuLabel =
        MenuLabel::create("YOU SCORED MORE THAN\n" + std::to_string(currentScoreThreshold) +
                              " POINTS!\n\n"
                              "FROM NOW ON,\n"
                              "THIS WILL BE YOUR\n"
                              "INITIAL SCORE IN THIS LEVEL.\n\n"
                              "NEXT THRESHOLD IS\n" +
                              std::to_string(nextScoreThreshold) + " POINTS",
                          {0.5, 0.55}, 0.03);
    menuLabel->setCascadeOpacityEnabled(true);
    menuLabel->setOpacity(0);

    addChild(menuLabel);
    menuLabel->runAction(FadeIn::create(1.0));

    PostLevelLayer::addDigitsPanel(*this, {0.5, 0.8}, "YOUR SCORE", currentScore);

    return true;
}

void GuaranteedScoreLayer::itemClicked(int itemIndex) { removeFromParentAndCleanup(true); }
