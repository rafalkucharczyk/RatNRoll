#include "AboutLayer.h"

#include "Menu/MenuLabel.h"
#include "Base/MipmapSprite.h"

#include <extensions/GUI/CCScrollView/CCScrollView.h>

USING_NS_CC;
using namespace std;

// #define ABOUT_LAYER_SPECIAL_BUTTONS

namespace
{

struct AboutEntry {
    string picture;
    string title;
    vector<string> names;

    static constexpr float pictureTotalHeight = 0.16; // of screen's height
    static constexpr float titleTotalHeight = 0.1;
    static constexpr float nameTotalHeight = 0.05;
};

vector<AboutEntry> aboutEntries = {
    {"menu/about/development.png", "DEVELOPMENT", {"Rafal Kucharczyk", "Lukasz Kucharczyk"}},
    {"menu/about/art.png", "ART & ANIMATION", {"Lukasz Kucharczyk", "Marta Kucharczyk"}},
    {"menu/about/music.png", "MUSIC & SFX", {"Lukasz Kucharczyk", "Foo Bar"}},
    {"menu/about/thanks.png", "SPECIAL THANKS", {"Adam Gryko"}},
    {"menu/about/cocos2dx_box2d.png", "POWERED BY", {"Cocos2dx", "BOX2D"}},
};
}

AboutLayer::AboutLayer()
    : menuHelper({{{0.5, 0.1}, 0.1, "back", {MenuHelper::removeTransparency()}},
#ifdef ABOUT_LAYER_SPECIAL_BUTTONS
                  {{0.9, 0.2}, 0.1, "play"},
                  {{0.9, 0.3}, 0.1, "settings"}
#endif
                 },
                 bind(&AboutLayer::menuItemClicked, this, placeholders::_1))
{
}

bool AboutLayer::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 232))) {
        return false;
    }

    addScrollView(composeScrollableLayer());

    addBackgroundItems();

    auto menu = menuHelper.getMenu();
    addChild(menu);

    return true;
}

float AboutLayer::getTotalHeightOfAboutEntries()
{
    float totalHeight = 0;
    for (const auto &entry : aboutEntries) {
        totalHeight += AboutEntry::pictureTotalHeight;
        totalHeight += AboutEntry::titleTotalHeight;
        totalHeight += AboutEntry::nameTotalHeight * entry.names.size();
    }

    return totalHeight;
}

Layer *AboutLayer::composeScrollableLayer()
{
    const float trailingVerticalHeight = 0.2;

    float totalHeight = getTotalHeightOfAboutEntries();

    Rect boundingBox = getBoundingBox();

    totalHeight += trailingVerticalHeight;

    Size scrollableLayerSize(boundingBox.size.width,
                             boundingBox.size.height * totalHeight + trailingVerticalHeight);

    Layer *scrollableLayer = Layer::create();
    scrollableLayer->setContentSize(scrollableLayerSize);

    for (const auto &entry : aboutEntries) {
        totalHeight -= AboutEntry::pictureTotalHeight;

        Sprite *sprite = MipmapSprite::create(entry.picture);
        MenuHelper::positionNode(*sprite, {0.5, totalHeight}, 0.125);
        scrollableLayer->addChild(sprite);

        totalHeight -= AboutEntry::titleTotalHeight;

        MenuLabel *title = MenuLabel::create(entry.title, {0.5, totalHeight}, 0.03);
        scrollableLayer->addChild(title);

        for (const string &name : entry.names) {
            totalHeight -= AboutEntry::nameTotalHeight;

            MenuLabel *title = MenuLabel::create(name, {0.5, totalHeight}, 0.03);
            scrollableLayer->addChild(title);
        }
    }

    return scrollableLayer;
}

void AboutLayer::addScrollView(cocos2d::Layer *scrollableLayer)
{
    Rect boundingBox = getBoundingBox();

    extension::ScrollView *scrollView =
        extension::ScrollView::create(boundingBox.size, scrollableLayer);
    scrollView->setDirection(extension::ScrollView::Direction::VERTICAL);

    scrollView->setContentOffset(scrollView->minContainerOffset());
    addChild(scrollView);

    runAction(Sequence::create(DelayTime::create(2), CallFunc::create([scrollView]() {
                                   scrollView->setContentOffsetInDuration(
                                       scrollView->maxContainerOffset(), 15.0);
                               }),
                               nullptr));
}

void AboutLayer::addBackgroundItems()
{
    Sprite *sprite = MipmapSprite::create("earth/earth01.png");
    MenuHelper::positionNode(*sprite, {0.4, 1.15}, 0.5);
    addChild(sprite);
    sprite->runAction(RepeatForever::create(RotateBy::create(20, 360)));

    sprite = MipmapSprite::create("earth/inner01_item01.png");
    MenuHelper::positionNode(*sprite, {0.725, 0.98}, 0.2);
    addChild(sprite);
    sprite->runAction(RepeatForever::create(RotateBy::create(10, 360)));

    sprite = MipmapSprite::create("earth/inner01_item02.png");
    MenuHelper::positionNode(*sprite, {0.4, 0}, 0.3);
    addChild(sprite);
    sprite->runAction(RepeatForever::create(RotateBy::create(10, -360)));

    sprite = MipmapSprite::create("earth/inner01_item01.png");
    MenuHelper::positionNode(*sprite, {0.675, 0.0}, 0.4);
    addChild(sprite);
    sprite->runAction(RepeatForever::create(RotateBy::create(15, 360)));
}

void AboutLayer::menuItemClicked(int itemIndex)
{
    if (itemIndex == 0 && gotoMainMenuCallback) {
        gotoMainMenuCallback();
    }

    if (itemIndex == 1 && gotoTestLayerCallback) {
        gotoTestLayerCallback();
    }

    if (itemIndex == 2 && resetGameStateCallback) {
        resetGameStateCallback();
    }
}
