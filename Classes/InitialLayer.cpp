#include "InitialLayer.h"

#include "AssetsPreloader.h"

USING_NS_CC;

InitialLayer::InitialLayer()
    : menuHelper({{{0.5, 0.625}, 0.35, "play"},
                  {{0.35, 0.25}, 0.125, "settings"},
                  {{0.65, 0.25}, 0.125, "gamecenter"}},
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
