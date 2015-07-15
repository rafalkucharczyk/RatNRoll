#include "PostLevelLayer.h"

USING_NS_CC;

PostLevelLayer::PostLevelLayer()
    : menuHelper({{{0.35, 0.2}, 0.1, "menu/menu.png"}, {{0.65, 0.2}, 0.1, "menu/restart.png"}},
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
    auto ratSprite = Sprite::create("rat.png");
    MenuHelper::positionNode(*ratSprite, {0.5, 0.72}, 0.05);
    addChild(ratSprite);

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
