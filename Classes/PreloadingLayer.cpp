#include "PreloadingLayer.h"

#include "AssetsPreloader.h"

#include "MipmapSprite.h"
#include "Menu/MenuLabel.h"

USING_NS_CC;
using namespace std;

PreloadingLayer::PreloadingLayer() : currentItemsCount(0) {}

bool PreloadingLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    Vec2 visibleSize(Director::getInstance()->getVisibleSize());

    auto backgroundImage = Sprite::create("background/bg_plane01.png");
    backgroundImage->setPosition(visibleSize / 2);
    backgroundImage->setScale(visibleSize.x / backgroundImage->getContentSize().width,
                              visibleSize.y / backgroundImage->getContentSize().height);
    addChild(backgroundImage);

    addChild(MenuLabel::create("LOADING...", {0.5, 0.3}, 0.03));

    filesToPreload = AssetsPreloader::list();

    scheduleOnce(std::bind(&PreloadingLayer::preload, this, placeholders::_1), 0.01,
                 "PreloadingLayer_preloading");

    return true;
}

void PreloadingLayer::preload(float t)
{
    static float filesPerItem = filesToPreload.size() / float(maxItemsCount);
    static int currentFileIndex = 0;

    AssetsPreloader::preload(filesToPreload.back());
    filesToPreload.pop_back();

    if (static_cast<int>(currentFileIndex / filesPerItem) >= currentItemsCount) {
        insertBackgroundItem();
        currentItemsCount++;
    }

    currentFileIndex++;

    if (!filesToPreload.empty()) {
        scheduleOnce(std::bind(&PreloadingLayer::preload, this, placeholders::_1), 0.01,
                     "PreloadingLayer_preloading" + std::to_string(filesToPreload.size()));
    } else {
        if (preloadingCompletedCallback) {
            preloadingCompletedCallback();
        }
    }
}

void PreloadingLayer::insertBackgroundItem()
{
    auto sprite = MipmapSprite::create("background/bg_item01.png");

    MenuHelper::positionNode(*sprite, {rand_0_1(), rand_0_1()}, 0.05);
    sprite->setScale(sprite->getScale() * (1 + 0.4 * rand_0_1()));
    sprite->setRotation(rand_0_1() * 360);

    addChild(sprite);
}
