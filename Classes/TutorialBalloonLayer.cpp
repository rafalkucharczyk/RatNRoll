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
    animationNode->setEventListener([](int trackIndex, spEvent* event) {
        if (std::string(event->data->name) == "score") {
            std::string mode((event->stringValue != nullptr) ? event->stringValue : "");
            int score = event->intValue;
            CCLOG("TUTORIAL, got anim event, score: %d, mode: %s", score, mode.c_str());
            if (mode == "instant") {
                //set score instantly, without digits animation
            } else {
                //set score with digits animations
            }
        }
    });
    MenuHelper::positionNode(*animationNode, getAnimationInitPos(), 0.15);
    addChild(animationNode);
}

std::string TutorialBalloonLayer::getAnimationName() const
{
    switch (balloonType) {
        case BalloonType::CONTROLS:
            return "tutorial00.x";
        case BalloonType::SPEEDUP:
            return "tutorial01.x";
        case BalloonType::SLOWDOWN:
            return "tutorial02.x";
        case BalloonType::HOVER:
            return "tutorial03.x";
        case BalloonType::HALVE:
            return "tutorial04.x";
        default:
            assert(false);
            return "";
    }
}

cocos2d::Vec2 TutorialBalloonLayer::getAnimationInitPos() const
{
    const float yPos = 0.6;
    switch (balloonType) {
        case BalloonType::CONTROLS:
            return cocos2d::Vec2(0.4, yPos);
        case BalloonType::SPEEDUP:
            return cocos2d::Vec2(0.65, yPos);
        case BalloonType::SLOWDOWN:
            return cocos2d::Vec2(0.25, yPos);
        case BalloonType::HOVER:
            return cocos2d::Vec2(0.25, yPos);
        case BalloonType::HALVE:
            return cocos2d::Vec2(0.3, yPos);
        default:
            return cocos2d::Vec2(0.0, yPos);
    }
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
