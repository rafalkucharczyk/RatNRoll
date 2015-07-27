#include "TutorialBalloonLayer.h"

#include <spine/spine-cocos2dx.h>

#include "MipmapSprite.h"

USING_NS_CC;

TutorialBalloonLayer::TutorialBalloonLayer(BalloonType balloonType,
                                           std::function<void()> closeCallback)
    : balloonType(balloonType), closeCallback(closeCallback),
      menuHelper({{{0.91, 0.95}, 0.06, "cancel", false}},
                 std::bind(&TutorialBalloonLayer::itemClicked, this, std::placeholders::_1))
{
}

TutorialBalloonLayer *TutorialBalloonLayer::create(BalloonType balloonType,
                                                   std::function<void()> closeCallback)
{
    TutorialBalloonLayer *ret = new (std::nothrow) TutorialBalloonLayer(balloonType, closeCallback);
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return NULL;
}

bool TutorialBalloonLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    EventListenerTouchOneByOne *listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [&](Touch *touch, Event *event) { return true; };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,
                                                                                          this);
    addBalloonSprite();

    runAction(Sequence::create(DelayTime::create(5),
                               CallFunc::create([this]() { this->selfCleanUp(); }), nullptr));

    return true;
}

void TutorialBalloonLayer::onExit()
{
    Layer::onExit();

    closeCallback();
}

void TutorialBalloonLayer::addBalloonSprite()
{
    auto balloonSprite = MipmapSprite::create("tutorial_balloon.png");
    MenuHelper::positionNode(*balloonSprite, {0.7, 0.8}, 0.3);

    addChild(balloonSprite);

    auto animationNode = spine::SkeletonAnimation::createWithFile("animations/animations.json",
                                                                  "animations/rat.atlas");
    animationNode->setAnimation(0, getAnimationName(), true);
    animationNode->updateWorldTransform();
    MenuHelper::positionNode(*animationNode, {0.7, 0.775}, 0.1);
    addChild(animationNode);
}

std::string TutorialBalloonLayer::getAnimationName()
{
    // TODO use correct animations

    if (balloonType == BalloonType::CONTROLS) {
        return "run_test01";
    } else if (balloonType == BalloonType::SPEEDUP) {
        return "jump01";
    } else if (balloonType == BalloonType::SLOWDOWN) {
        return "run_test01";
    } else if (balloonType == BalloonType::HOVER) {
        return "jump01";
    } else if (balloonType == BalloonType::HALVE) {
        return "run_test01";
    }

    assert(false);
    return "";
}

void TutorialBalloonLayer::itemClicked(int itemIndex) { selfCleanUp(); }

void TutorialBalloonLayer::selfCleanUp() { removeFromParentAndCleanup(true); }
