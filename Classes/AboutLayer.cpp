#include "AboutLayer.h"

#include "Menu/MenuLabel.h"
#include "Base/MipmapSprite.h"

USING_NS_CC;

AboutLayer::AboutLayer()
    : menuHelper({{{0.9, 0.2}, 0.1, "settings"}, {{0.5, 0.2}, 0.1, "back"}},
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

    MenuLabel *label = MenuLabel::create("LUKASZ\nKUCHARCZYK\n\n"
                                         "MARTA\nKUCHARCZYK\n\n"
                                         "RAFAL\nKUCHARCZYK",
                                         {0.5, 0.55}, 0.03);

    addChild(label);

    return true;
}

void AboutLayer::menuItemClicked(int itemIndex)
{
    if (itemIndex == 0 && resetGameStateCallback) {
        resetGameStateCallback();
    }

    if (itemIndex == 1 && gotoMainMenuCallback) {
        gotoMainMenuCallback();
    }
}
