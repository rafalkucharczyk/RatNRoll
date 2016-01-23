#include "GuaranteedScoreLayer.h"

#include "Menu/MenuLabel.h"

#include "PostLevelLayer.h"

USING_NS_CC;

GuaranteedScoreLayer::GuaranteedScoreLayer(int currentScore, int currentScoreThreshold,
                                           int nextScoreThreshold)
    : currentScore(currentScore), currentScoreThreshold(currentScoreThreshold),
      nextScoreThreshold(nextScoreThreshold),
      menuHelper({{{0.5, 0.19}, 0.1, "confirm", {MenuHelper::removeTransparency()}}},
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

    auto menuLabelTop = MenuLabel::create("YOU'VE SCORED OVER\n" +
                                              std::to_string(currentScoreThreshold) + " POINTS!",
                                          {0.5, 0.68}, 0.03);
    menuLabelTop->setCascadeOpacityEnabled(true);
    menuLabelTop->setOpacity(0);

    addChild(menuLabelTop);
    menuLabelTop->runAction(FadeIn::create(1.0));

    addAnimationNode();
    addFacebookNode();

    if (nextScoreThreshold != 0) {
        auto menuLabelBottom = MenuLabel::create(
            "NOW SCORE\n" + std::to_string(nextScoreThreshold) + " POINTS!", {0.5, 0.45}, 0.03);
        menuLabelBottom->setCascadeOpacityEnabled(true);
        menuLabelBottom->setOpacity(0);

        addChild(menuLabelBottom);
        menuLabelBottom->runAction(FadeIn::create(1.0));
    }

    PostLevelLayer::addDigitsPanel(*this, {0.5, 0.8}, "YOUR SCORE", currentScore);

    return true;
}

void GuaranteedScoreLayer::itemClicked(int itemIndex) { removeFromParentAndCleanup(true); }

void GuaranteedScoreLayer::addAnimationNode()
{
    auto ratNode = spine::SkeletonAnimation::createWithFile("animations/rat/skeleton.json",
                                                            "animations/rat/skeleton.atlas");
    ratNode->setAnimation(0, "post_happy_01.x", true);
    ratNode->updateWorldTransform();
    MenuHelper::positionNode(*ratNode, {0.5, 0.52}, 0.12);
    addChild(ratNode);
}

void GuaranteedScoreLayer::addFacebookNode()
{
    addChild(MenuLabel::create("HAVING FUN? LIKE US!", {0.5, 0.35}, 0.02));

    facebookLikeNode = FacebookLikeNode::create("1775466602680654", {0.5, 0.31}, 0);
    addChild(facebookLikeNode);
}
