#include "InitialLayer.h"

USING_NS_CC;

InitialLayer::InitialLayer()
    : menuHelper({"Tutorial", "Level 1"},
                 std::bind(&InitialLayer::itemClicked, this, std::placeholders::_1))
{
}

bool InitialLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    addChild(menuHelper.getMenu());

    return true;
}
