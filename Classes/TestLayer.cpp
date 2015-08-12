#ifdef COCOS2D_DEBUG

#include "TestLayer.h"

#include <iomanip>

#include <spine/spine-cocos2dx.h>

#include "MenuHelper.h"

USING_NS_CC;

TestLayer::TestLayer() {}

bool TestLayer::init()
{
    if (!LayerColor::initWithColor(Color4B::WHITE)) {
        return false;
    }

    auto animationNode = spine::SkeletonAnimation::createWithFile("animations/animations.json",
                                                                  "animations/rat.atlas");

    std::string slowName = "run02.x";
    std::string fastName = "run06.x";
    animationNode->setMix(slowName, fastName, 0.25);
    animationNode->setMix(fastName, slowName, 0.25);
    animationNode->updateWorldTransform();
    MenuHelper::positionNode(*animationNode, {0.5, 0.6}, 0.2);
    addChild(animationNode);

    animationNode->setTimeScale(0.5);

    animationNode->setAnimation(0, fastName, true);

    animationNode->runAction(RepeatForever::create(
        Sequence::create(DelayTime::create(2.47), CallFunc::create([=]() {
                             animationNode->setCompleteListener([=](int trackIndex, int) {
                                 if (trackIndex == 0) {
                                     animationNode->setAnimation(0, slowName, true);
                                 };
                             });
                         }),
                         DelayTime::create(3.15), CallFunc::create([=]() {
                             animationNode->setCompleteListener([=](int trackIndex, int) {
                                 if (trackIndex == 0) {
                                     animationNode->setAnimation(0, fastName, true);
                                 };
                             });
                         }),

                         nullptr)));

    animationNode->runAction(RepeatForever::create(
        Sequence::create(CallFunc::create([this, animationNode]() {
                             animationNode->setAnimation(1, getEyeAnimation(), false);
                         }),
                         DelayTime::create(3.0), nullptr)));

    return true;
}

std::string TestLayer::getEyeAnimation()
{
    static int i = 1;

    std::ostringstream os;
    os << "eyes" << std::setfill('0') << std::setw(2) << i++ << ".x";

    if (i == 12) {
        i = 1;
    }

    return os.str();
}

#endif // COCOS2D_DEBUG