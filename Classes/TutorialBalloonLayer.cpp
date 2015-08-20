#include "TutorialBalloonLayer.h"

#include "MipmapSprite.h"

USING_NS_CC;

TutorialBalloonLayer::TutorialBalloonLayer(BalloonType balloonType,
                                           std::function<void()> closeCallback)
    : balloonType(balloonType), closeCallback(closeCallback),
      menuHelper({{{0.91, 0.95}, 0.06, "cancel", {MenuHelper::removeTransparency()}}},
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
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 216))) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    EventListenerTouchOneByOne *listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [&](Touch *touch, Event *event) { return true; };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,
                                                                                          this);

    animationNode = spine::SkeletonAnimation::createWithFile("animations/animations.json",
                                                             "animations/rat.atlas");

    addBalloonSprite();

    runAction(Sequence::create(DelayTime::create(getAnimationDuration(getAnimationName())),
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
    animationNode->setAnimation(0, getAnimationName(), false);
    animationNode->updateWorldTransform();
    MenuHelper::positionNode(*animationNode, {0.7, 0.6}, 0.15);
    addChild(animationNode);
}

std::string TutorialBalloonLayer::getAnimationName()
{
    // TODO use correct animations

    if (balloonType == BalloonType::CONTROLS) {
        return "run06.x";
    } else if (balloonType == BalloonType::SPEEDUP) {
        return "tutorial01.x";
    } else if (balloonType == BalloonType::SLOWDOWN) {
        return "tutorial02.x";
    } else if (balloonType == BalloonType::HOVER) {
        return "tutorial03.x";
    } else if (balloonType == BalloonType::HALVE) {
        return "run05.x";
    }

    assert(false);
    return "";
}

float TutorialBalloonLayer::getAnimationDuration(const std::string &animationName)
{
    spSkeletonData *const skeletonData = animationNode->getState()->data->skeletonData;

    for (int i = 0; i < skeletonData->animationsCount; i++) {
        if (skeletonData->animations[i]->name == animationName) {
            return skeletonData->animations[i]->duration;
        }
    }

    return 0.0;
}

void TutorialBalloonLayer::itemClicked(int itemIndex) { selfCleanUp(); }

void TutorialBalloonLayer::selfCleanUp() { removeFromParentAndCleanup(true); }
