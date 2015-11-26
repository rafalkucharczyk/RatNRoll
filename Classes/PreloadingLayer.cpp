#include "PreloadingLayer.h"

#include "AssetsPreloader.h"

#include "MipmapSprite.h"
#include "Menu/MenuLabel.h"

USING_NS_CC;

PreloadingLayer::PreloadingLayer() {}

bool PreloadingLayer::init()
{
    if (!LayerColor::initWithColor(Color4B::WHITE)) {
        return false;
    }

    Sprite *sprite = MipmapSprite::create("earth/rat.png");
    MenuHelper::positionNode(*sprite, {0.5, 0.6}, 0.3);
    addChild(sprite);

    addChild(MenuLabel::create("LOADING...", {0.5, 0.3}, 0.03));

    scheduleOnce([this](float f) {
        AssetsPreloader::preload();
        if (preloadingCompletedCallback) {
            preloadingCompletedCallback();
        }
    }, 0.25, "PreloadingLayer_preloading");

    return true;
}
