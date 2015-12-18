#include "PreloadingLayer.h"

#include "AssetsPreloader.h"

#include "MipmapSprite.h"
#include "Menu/MenuLabel.h"

#include "BackgroundLayer.h"

USING_NS_CC;
using namespace std;

namespace
{

BackgroundItemsInitialState backgroundItemsInitialState = {
    {"", {0.34, 0.79}, 0, 0}, {"", {0.92, 0.89}, 0, 0}, {"", {0.19, 0.12}, 0, 0},
    {"", {0.50, 0.63}, 0, 0}, {"", {0.20, 0.62}, 0, 0}, {"", {0.89, 0.53}, 0, 0},
    {"", {0.63, 0.85}, 0, 0}, {"", {0.70, 0.20}, 0, 0}, {"", {0.10, 0.85}, 0, 0},
    {"", {0.96, 0.25}, 0, 0}, {"", {0.36, 0.04}, 0, 0}, {"", {0.90, 0.11}, 0, 0},
    {"", {0.03, 0.44}, 0, 0}, {"", {0.30, 0.55}, 0, 0}, {"", {0.78, 0.75}, 0, 0},
    {"", {0.67, 0.59}, 0, 0}, {"", {0.51, 0.09}, 0, 0}, {"", {0.50, 0.95}, 0, 0},
    {"", {0.33, 0.93}, 0, 0}, {"", {0.18, 0.96}, 0, 0}, {"", {0.93, 0.69}, 0, 0},
    {"", {0.92, 0.32}, 0, 0}, {"", {0.48, 0.69}, 0, 0}, {"", {0.29, 0.44}, 0, 0},
    {"", {0.13, 0.72}, 0, 0},
};
}

PreloadingLayer::PreloadingLayer() : currentItemsCount(0) {}

bool PreloadingLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    addBackgroundImage();

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
        backgroundItemsInitialState.erase(
            std::remove_if(
                backgroundItemsInitialState.begin(), backgroundItemsInitialState.end(),
                [](const BackgroundItemInitialState &item) { return item.fileName.empty(); }),
            backgroundItemsInitialState.end());
        BackgroundLayer::setBackgroundItemsInitialState(backgroundItemsInitialState);
        if (preloadingCompletedCallback) {
            preloadingCompletedCallback();
        }
    }
}

void PreloadingLayer::addBackgroundImage()
{
    Vec2 visibleSize(Director::getInstance()->getVisibleSize());

    auto backgroundImage = Application::getInstance()->getSplashScreen();

    if (backgroundImage == nullptr) {
        backgroundImage = Sprite::create("background/bg_plane01.png");
        backgroundImage->setScale(visibleSize.x / backgroundImage->getContentSize().width,
                                  visibleSize.y / backgroundImage->getContentSize().height);
    }

    backgroundImage->setPosition(visibleSize / 2);
    addChild(backgroundImage);
}

void PreloadingLayer::insertBackgroundItem(int index)
{
    if (index >= backgroundItemsInitialState.size()) {
        return;
    }

    backgroundItemsInitialState[index].fileName = "background/bg_item01.png";
    auto sprite = MipmapSprite::create(backgroundItemsInitialState[index].fileName);

    MenuHelper::positionNode(*sprite, backgroundItemsInitialState[index].position, 0.05);

    Size visibleSize(Director::getInstance()->getVisibleSize());

    backgroundItemsInitialState[index].position.x =
        backgroundItemsInitialState[index].position.x * visibleSize.width;
    backgroundItemsInitialState[index].position.y =
        backgroundItemsInitialState[index].position.y * visibleSize.height;

    backgroundItemsInitialState[index].scale = 1 + 0.5 * rand_0_1();
    sprite->setScale(sprite->getScale() * backgroundItemsInitialState[index].scale);

    backgroundItemsInitialState[index].rotation = rand_0_1() * 360;
    sprite->setRotation(backgroundItemsInitialState[index].rotation);

    addChild(sprite);
}
