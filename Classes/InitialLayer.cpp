#include "InitialLayer.h"

USING_NS_CC;

InitialLayer::InitialLayer()
    : menuHelper({{{0.2, 0.2}, 0.1, "tutorial"},
                  {{0.5, 0.55}, 0.35, "play"},
                  {{0.4, 0.2}, 0.1, "settings"},
                  {{0.6, 0.2}, 0.1, "gamecenter"},
                  {{0.8, 0.2}, 0.1, "about"}},
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
