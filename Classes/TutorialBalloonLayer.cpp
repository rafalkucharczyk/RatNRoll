#include "TutorialBalloonLayer.h"

#include "MipmapSprite.h"
#include "Menu/MenuLabel.h"

USING_NS_CC;

namespace
{
std::map<TutorialBalloonLayer::BalloonType, std::string> tutorialTexts = {
    {TutorialBalloonLayer::BalloonType::CONTROLS, "ROLL THE WORLD AND KEEP\n"
                                                  "YOURSELF ON TOP OF IT.\n"
                                                  "THE MORE YOU ROLL,\n"
                                                  "THE MORE YOU SCORE!\n\n"
                                                  "OH... AND REMEMBER TO\n"
                                                  "ALWAYS ROLL RIGHT,\n"
                                                  "YOU GET NOTHING WHEN\n"
                                                  "ROLLING LEFT"},
    {TutorialBalloonLayer::BalloonType::SPEEDUP, "CATCH VIALS TO\n"
                                                 "SPEED YOU UP!\n\n"
                                                 "YOU GET MORE POINTS\n"
                                                 "WHEN RUNNING FASTER.\n"
                                                 "AND MORE FUN!"},
    {TutorialBalloonLayer::BalloonType::SLOWDOWN, "AVOID HAMBURGERS,\n"
                                                  "THEY SLOW YOU DOWN!\n\n"
                                                  "YOU GET LESS POINTS THEN..."},
    {TutorialBalloonLayer::BalloonType::HOVER, "GOOSE MAKES YOU FLY FOR\n"
                                               "A LITTLE WHILE,\n"
                                               "SO YOU CAN ROLL THE WORLD\n"
                                               "AS FAST AS YOU CAN!\n\n"
                                               "BUT BE CAREFUL WHEN\n"
                                               "LANDING, THOUGH"},
    {TutorialBalloonLayer::BalloonType::HALVE, "ALWAYS STAY CLEAR OF SKULLS\n"
                                               "THEY DO NOT KILL YOU, BUT THEY\n"
                                               "TAKE HALF OF YOUR POINTS!\n"
                                               "IT HURTS EVEN MORE..."}};
}

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

    addText();
    addDigitsPanel();
    addAnimationSprite();

    runAction(Sequence::create(DelayTime::create(getAnimationDuration(getAnimationName())),
                               CallFunc::create([this]() { this->selfCleanUp(); }), nullptr));

    return true;
}

void TutorialBalloonLayer::onExit()
{
    Layer::onExit();

    closeCallback();
}

void TutorialBalloonLayer::addText()
{
    auto menuLabel = MenuLabel::create(tutorialTexts[balloonType], {0.5, 0.3}, 0.03);
    menuLabel->setCascadeOpacityEnabled(true);
    menuLabel->setOpacity(0);

    addChild(menuLabel);
    menuLabel->runAction(FadeIn::create(1.0));
}

void TutorialBalloonLayer::addDigitsPanel()
{
    digitsPanel = DigitsPanel::createWithNumberOfDigits(4);
    digitsPanel->setVisible(false);
    addChild(digitsPanel);
    MenuHelper::positionNode(*digitsPanel, {0.7, 0.5}, 0.04);
}

void TutorialBalloonLayer::addAnimationSprite()
{
    animationNode = spine::SkeletonAnimation::createWithFile("animations/rat/skeleton.json",
                                                             "animations/rat/skeleton.atlas");

    animationNode->setAnimation(0, getAnimationName(), false);
    animationNode->updateWorldTransform();
    animationNode->setEventListener([this](int trackIndex, spEvent *event) {
        if (std::string(event->data->name) == "score") {
            digitsPanel->setVisible(true);

            std::string mode((event->stringValue != nullptr) ? event->stringValue : "");
            int score = event->intValue;

            if (mode == "instant") {
                digitsPanel->setNumber(score);
            } else {
                digitsPanel->animateToNumber(score);
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

float TutorialBalloonLayer::getAnimationDuration(const std::string &animationName) const
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
