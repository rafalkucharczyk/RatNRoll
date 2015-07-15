#include "InitialLayer.h"

USING_NS_CC;

InitialLayer::InitialLayer()
    : menuHelper({{{0.35, 0.2}, 0.1, "menu/tutorial.png"},
                  {{0.5, 0.55}, 0.35, "menu/play.png"},

                  {{0.65, 0.2}, 0.1, "menu/gamecenter.png"}},
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
