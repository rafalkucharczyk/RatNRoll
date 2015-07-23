#include "LevelMenuLayer.h"

USING_NS_CC;

LevelMenuLayer::LevelMenuLayer()
    : menuHelper({{{0.92, 0.96}, 0.05, "pause"}},
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
