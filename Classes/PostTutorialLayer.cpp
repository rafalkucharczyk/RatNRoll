#include "PostTutorialLayer.h"

#include <spine/spine-cocos2dx.h>

#include "Base/MipmapSprite.h"
#include "Menu/MenuLabel.h"

USING_NS_CC;

PostTutorialLayer::PostTutorialLayer()
    : menuHelper({{{0.35, 0.23}, 0.12, "back"}, {{0.65, 0.23}, 0.12, "restart"}},
                 std::bind(&PostTutorialLayer::menuItemClicked, this, std::placeholders::_1))
{
}

bool PostTutorialLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    addAnimation();

    return true;
}

void PostTutorialLayer::setRestartLevelCallback(std::function<void()> callback)
{
    restartLevelCallback = callback;
}

void PostTutorialLayer::setGotoMainMenuCallback(std::function<void()> callback)
{
    gotoMainMenuCallback = callback;
}

void PostTutorialLayer::menuItemClicked(int itemIndex)
{
    if (itemIndex == 0 && gotoMainMenuCallback) {
        gotoMainMenuCallback();
    }

    if (itemIndex == 1 && restartLevelCallback) {
        restartLevelCallback();
    }
}

void PostTutorialLayer::addAnimation()
{
    auto ratNode = spine::SkeletonAnimation::createWithFile("animations/rat/skeleton.json",
                                                            "animations/rat/skeleton.atlas");
    addChild(MenuLabel::create("OH!...\n\n"
                               "PLEASE TRY AGAIN",
                               {0.5, 0.7}, 0.03));
    ratNode->setAnimation(0, "post_sad_01.x", true);
    ratNode->updateWorldTransform();
    MenuHelper::positionNode(*ratNode, {0.5, 0.43}, 0.15);
    addChild(ratNode);
}
