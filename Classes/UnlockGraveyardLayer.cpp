#include "UnlockGraveyardLayer.h"

#include "FacebookLikeNode.h"

#include "Menu/MenuLabel.h"
#include "Base/MipmapSprite.h"

USING_NS_CC;

UnlockGraveyardLayer::UnlockGraveyardLayer() {}

bool UnlockGraveyardLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    setCascadeOpacityEnabled(true);

    addChild(MenuLabel::create("SCORE 10000 POINTS\n"
                               "TO UNLOCK THIS LEVEL",
                               {0.5, 0.85}, 0.03));

    addChild(MenuLabel::create("AND GET A NEW ITEM", {0.5, 0.75}, 0.03));

    Sprite *helmetSprite = MipmapSprite::create("items/shield01.png");
    MenuHelper::positionNode(*helmetSprite, {0.5, 0.67}, 0.075);
    addChild(helmetSprite);
    auto scaleAction = ScaleBy::create(0.5, 1.5);
    helmetSprite->runAction(
        RepeatForever::create(Sequence::create(scaleAction, scaleAction->reverse(), nullptr)));

    addChild(MenuLabel::create("HELMET\n\n"
                               "PROTECTS YOU\n"
                               "AGAINST SKULLS",
                               {0.5, 0.55}, 0.03));

    addChild(MenuLabel::create("HAVING FUN? LIKE US!", {0.5, 0.35}, 0.02));

    facebookLikeNode = FacebookLikeNode::create("1775466602680654", {0.5, 0.31}, 0);
    addChild(facebookLikeNode);

    return true;
}
