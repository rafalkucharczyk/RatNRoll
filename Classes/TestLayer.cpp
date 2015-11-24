#ifdef COCOS2D_DEBUG

#include "TestLayer.h"

#include <iomanip>

#include <spine/spine-cocos2dx.h>

#include "MenuHelper.h"

USING_NS_CC;

TestLayer::TestLayer()
    : menuHelper({{{0.5, 0.2}, 0.1, "back"}},
                 std::bind(&TestLayer::menuItemClicked, this, std::placeholders::_1))
{
}

bool TestLayer::init()
{
    if (!LayerColor::initWithColor(Color4B::WHITE)) {
        return false;
    }

    addChild(menuHelper.getMenu());

    auto animationNode = spine::SkeletonAnimation::createWithFile("animations/rat/skeleton.json",
                                                                  "animations/rat/skeleton.atlas");

    std::string slowName = "run05.x";
    std::string fastName = "run06.x";
    animationNode->setMix(slowName, fastName, 0.25);
    animationNode->setMix(fastName, slowName, 0.25);
    animationNode->updateWorldTransform();
    MenuHelper::positionNode(*animationNode, {0.5, 0.6}, 0.2);
    addChild(animationNode);

    animationNode->setTimeScale(1);

    animationNode->setAnimation(0, fastName, true);

    animationNode->runAction(RepeatForever::create(
        Sequence::create(DelayTime::create(3), CallFunc::create([=]() {
                             animationNode->setCompleteListener([=](int trackIndex, int) {
                                 if (trackIndex == 0) {
                                     animationNode->setAnimation(0, slowName, true);
                                 };
                             });
                         }),
                         DelayTime::create(3), CallFunc::create([=]() {
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

void TestLayer::menuItemClicked(int itemIndex)
{
    if (gotoMainMenuCallback) {
        gotoMainMenuCallback();
    }
}

#endif // COCOS2D_DEBUG
