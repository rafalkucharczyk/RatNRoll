#include "PauseLayer.h"

USING_NS_CC;

PauseLayer::PauseLayer()
    : menuHelper({{{0.5, 0.625}, 0.35, "play"}, {{0.5, 0.325}, 0.1, "back"}},
                 std::bind(&PauseLayer::itemClicked, this, std::placeholders::_1))
{
}

bool PauseLayer::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 216))) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    return true;
}

void PauseLayer::itemClicked(int itemIndex)
{
    if (itemIndex == 0 && gameResumedCallback) {
        gameResumedCallback();
    }

    if (itemIndex == 1 && gameQuitCallback) {
        gameQuitCallback();
    }
}
