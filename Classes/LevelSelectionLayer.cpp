#include "LevelSelectionLayer.h"

USING_NS_CC;

LevelSelectionLayer::LevelSelectionLayer()
    : menuHelper({{{0.3 + 0.01, 0.7 + 0.01}, 0.2, "tutorial"},
                  {{0.7 + 0.01, 0.7 - 0.01}, 0.2, "cheese_factory"},
                  {{0.3 - 0.02, 0.4 + 0.02}, 0.2, "graveyard"},
                  {{0.7 - 0.01, 0.4 - 0.01}, 0.2, "magic_hall"},
                  {{0.5, 0.2}, 0.1, "back"}},
                 std::bind(&LevelSelectionLayer::itemClicked, this, std::placeholders::_1))
{
}

bool LevelSelectionLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    addChild(menuHelper.getMenu());

    return true;
}
