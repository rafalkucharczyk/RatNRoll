#include "PostLevelLayer.h"

USING_NS_CC;

PostLevelLayer::PostLevelLayer()
    : menuHelper({"Restart", "Main Menu"},
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
    Size visibleSize = Director::getInstance()->getVisibleSize();

    bestScoreLabel = Label::createWithSystemFont("0", "Marker Felt", 40);
    bestScoreLabel->setColor(Color3B::BLACK);
    bestScoreLabel->setPosition(visibleSize.width * 0.5, visibleSize.height * 0.25);

    addChild(bestScoreLabel);

    return true;
}

void PostLevelLayer::displayBestScore(int score)
{
    bestScoreLabel->setString("Best score: " + std::to_string(score));
}

void PostLevelLayer::menuItemClicked(int itemIndex)
{
    if (itemIndex == 0 && restartLevelCallback) {
        restartLevelCallback();
    }

    if (itemIndex == 1 && gotoMainMenuCallback) {
        gotoMainMenuCallback();
    }
}
