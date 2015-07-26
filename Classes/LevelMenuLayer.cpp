#include "LevelMenuLayer.h"

USING_NS_CC;

LevelMenuLayer::LevelMenuLayer()
    : menuHelper({{{0.91, 0.95}, 0.06, "pause"}},
                 std::bind(&LevelMenuLayer::itemClicked, this, std::placeholders::_1))
{
}

bool LevelMenuLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    return true;
}
