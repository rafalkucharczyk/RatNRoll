#include "AboutLayer.h"

USING_NS_CC;

AboutLayer::AboutLayer()
    : menuHelper({{{0.5, 0.625}, 0.1, "settings"}, {{0.5, 0.325}, 0.1, "back"}},
                 std::bind(&AboutLayer::menuItemClicked, this, std::placeholders::_1))
{
}

bool AboutLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    return true;
}

void AboutLayer::menuItemClicked(int itemIndex)
{
    if (itemIndex == 0 && resetGameStateCallback) {
        resetGameStateCallback();
    }

    if (itemIndex == 1 && gotoMainMenuCallback) {
        gotoMainMenuCallback();
    }
}
