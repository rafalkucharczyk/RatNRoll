#include "PostLevelLayer.h"

#include <spine/spine-cocos2dx.h>

USING_NS_CC;

PostLevelLayer::PostLevelLayer()
    : menuHelper({{{0.2, 0.2}, 0.1, "back"},
                  {{0.4, 0.2}, 0.1, "restart"},
                  {{0.6, 0.2}, 0.1, "twitter"},
                  {{0.8, 0.2}, 0.1, "facebook"}},
                 std::bind(&PostLevelLayer::menuItemClicked, this, std::placeholders::_1))
{
}

bool PostLevelLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    // ---
    auto menu = menuHelper.getMenu();
    addChild(menu);

    // ---
    auto ratNode = spine::SkeletonAnimation::createWithFile("animations/skeleton.json",
                                                            "animations/skeleton.atlas");
    ratNode->setAnimation(0, "jump01", true);
    ratNode->updateWorldTransform();
    MenuHelper::positionNode(*ratNode, {0.5, 0.7}, 0.05);
    addChild(ratNode);

    currentScoreLabel = Label::createWithSystemFont("0", "Marker Felt", 120);
    currentScoreLabel->setColor(Color3B::BLACK);
    MenuHelper::positionNode(*currentScoreLabel, {0.5, 0.65}, 0.1);

    addChild(currentScoreLabel);

    // ---
    auto crownSprite = Sprite::create("menu/gamecenter.png");
    MenuHelper::positionNode(*crownSprite, {0.5, 0.505}, 0.05);
    addChild(crownSprite);

    bestScoreLabel = Label::createWithSystemFont("0", "Marker Felt", 120);
    bestScoreLabel->setColor(Color3B::BLACK);
    MenuHelper::positionNode(*bestScoreLabel, {0.5, 0.45}, 0.04);

    addChild(bestScoreLabel);

    return true;
}

void PostLevelLayer::displayCurrentScore(int score)
{
    currentScoreLabel->setString(std::to_string(score));
}

void PostLevelLayer::displayBestScore(int score)
{
    bestScoreLabel->setString(std::to_string(score));
}

void PostLevelLayer::menuItemClicked(int itemIndex)
{
    if (itemIndex == 0 && gotoMainMenuCallback) {
        gotoMainMenuCallback();
    }

    if (itemIndex == 1 && restartLevelCallback) {
        restartLevelCallback();
    }
}
