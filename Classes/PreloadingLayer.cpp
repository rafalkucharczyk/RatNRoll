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

    auto loadingImage = MipmapSprite::create("menu/loading.png");
    loadingImage->setPosition(visibleSize * 0.5);
    loadingImage->setScale(visibleSize.x / loadingImage->getContentSize().width * 0.45);
    addChild(loadingImage);

    addChild(MenuLabel::create("LOADING", {0.5, 0.27}, 0.03));

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
        insertBackgroundItem(currentItemsCount);
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

void PreloadingLayer::insertBackgroundItem(int no)
{
    const cocos2d::Vec2 posPresets[] = {
        {0.70, 0.20}, {0.36, 0.17}, {0.19, 0.89}, {0.83, 0.77}, {0.09, 0.48},
        {0.69, 0.53}, {0.92, 0.89}, {0.09, 0.63}, {0.34, 0.79}, {0.89, 0.48},
        {0.63, 0.79}, {0.80, 0.11}, {0.18, 0.32}, {0.30, 0.55}, {0.78, 0.95},
        {0.75, 0.33}, {0.50, 0.85}, {0.56, 0.11}, {0.33, 0.93}, {0.12, 0.08},
        {0.93, 0.69}, {0.92, 0.32}, {0.48, 0.69}, {0.29, 0.44}, {0.13, 0.72}
    };
    const size_t presetsSize = (sizeof(posPresets)/sizeof(cocos2d::Vec2));

    auto sprite = MipmapSprite::create("background/bg_item01.png");

    auto itemPos = posPresets[no % presetsSize];
    if (no/presetsSize > 0) {
        // we run out of prests, adjust the position with a random delta
        const cocos2d::Vec2 delta = cocos2d::Vec2(0.386, 0.581) +
        cocos2d::Vec2(0.219, 0.327)*rand_0_1();
        itemPos += delta*(no/presetsSize);
        itemPos.x = itemPos.x - floor(itemPos.x);
        itemPos.y = itemPos.y - floor(itemPos.y);
    }

    MenuHelper::positionNode(*sprite, itemPos, 0.05);
    sprite->setScale(sprite->getScale() * (1 + 0.5 * rand_0_1()));
    sprite->setRotation(rand_0_1() * 360);

    addChild(sprite);

}
