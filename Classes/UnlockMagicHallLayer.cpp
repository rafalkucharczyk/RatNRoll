#include "UnlockMagicHallLayer.h"

#include "Menu/MenuLabel.h"

USING_NS_CC;

bool UnlockMagicHallLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    setCascadeOpacityEnabled(true);

    addChild(MenuLabel::create("BUY THIS LEVEL", {0.5, 0.9}, 0.03));

    addChild(MenuLabel::create("AND GET A NEW ITEM", {0.5, 0.65}, 0.03));

    Sprite *frenzySprite = Sprite::create("frenzy01.png");
    MenuHelper::positionNode(*frenzySprite, {0.5, 0.58}, 0.075);
    addChild(frenzySprite);
    auto scaleAction = ScaleBy::create(0.5, 1.5);
    frenzySprite->runAction(
        RepeatForever::create(Sequence::create(scaleAction, scaleAction->reverse(), nullptr)));

    addChild(MenuLabel::create("CHESSE FRENZY\n\n"
                               "BOOSTS YOUR POINTS\n"
                               "FOR A FEW SECONDS",
                               {0.5, 0.45}, 0.03));

    addChild(MenuLabel::create("THE PURCHASE ALSO\n"
                               "REMOVES ALL ADS",
                               {0.5, 0.32}, 0.02));

    return true;
}
