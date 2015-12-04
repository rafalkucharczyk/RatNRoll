#include "GameCompletedLayer.h"

#include <spine/spine-cocos2dx.h>
#include "Menu/MenuLabel.h"

USING_NS_CC;

GameCompletedLayer::GameCompletedLayer(int levelNumber)
    : menuHelper({{{0.5, 0.2}, 0.1, "back"}},
                 std::bind(&GameCompletedLayer::itemClicked, this, std::placeholders::_1)),
      levelNumber(levelNumber)
{
}

GameCompletedLayer *GameCompletedLayer::create(int levelNumber)
{
    GameCompletedLayer *ret = new (std::nothrow) GameCompletedLayer(levelNumber);
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }

    return ret;
}

bool GameCompletedLayer::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 232))) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    addChild(MenuLabel::create("LEVEL COMPLETED", {0.5, 0.8}, 0.03));

    auto ratNode = spine::SkeletonAnimation::createWithFile("animations/rat/skeleton.json",
                                                            "animations/rat/skeleton.atlas");

    ratNode->setAnimation(0, "post_happy_01.x", true);
    ratNode->updateWorldTransform();
    MenuHelper::positionNode(*ratNode, {0.5, 0.4}, 0.25);
    addChild(ratNode);

    return true;
}

void GameCompletedLayer::itemClicked(int itemIndex)
{
    if (itemIndex == 0 && completionConifrmedCallback) {
        completionConifrmedCallback();
    }
}
