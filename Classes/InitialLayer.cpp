#include "InitialLayer.h"

#include "AssetsPreloader.h"

USING_NS_CC;

InitialLayer::InitialLayer()
    : menuHelper({{{0.5, 0.625}, 0.35, "play"},
                  {{0.3, 0.275}, 0.1, "settings"},
                  {{0.5, 0.275}, 0.1, "gamecenter"},
                  {{0.7, 0.275}, 0.1, "about"}},

                 std::bind(&InitialLayer::itemClicked, this, std::placeholders::_1))
{
}

bool InitialLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    addChild(menuHelper.getMenu());

    scheduleOnce([](float f) {
        AssetsPreloader::preload();
    }, 10, "preloading");

    return true;
}
