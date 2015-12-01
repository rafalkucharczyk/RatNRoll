#include "AboutLayer.h"

#include "Menu/MenuLabel.h"
#include "Base/MipmapSprite.h"

USING_NS_CC;

AboutLayer::AboutLayer()
    : menuHelper(
          {{{0.9, 0.2}, 0.1, "settings"}, {{0.9, 0.3}, 0.1, "play"}, {{0.5, 0.2}, 0.1, "back"}},
          std::bind(&AboutLayer::menuItemClicked, this, std::placeholders::_1))
{
}

bool AboutLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    Sprite *sprite = MipmapSprite::create("menu/credits.png");
    MenuHelper::positionNode(*sprite, {0.5, 0.75}, 0.1);
    addChild(sprite);

    MenuLabel *label = MenuLabel::create("£UKASZ\nKUCHARCZYK\n\n"
                                         "MARTA\nKUCHARCZYK\n\n"
                                         "RAFA£\nKUCHARCZYK",
                                         {0.5, 0.55}, 0.03);

    addChild(label);

    return true;
}

void AboutLayer::menuItemClicked(int itemIndex)
{
    if (itemIndex == 0 && resetGameStateCallback) {
        resetGameStateCallback();
    }

    if (itemIndex == 1 && gotoTestLayerCallback) {
        gotoTestLayerCallback();
    }

    if (itemIndex == 2 && gotoMainMenuCallback) {
        gotoMainMenuCallback();
    }
}
