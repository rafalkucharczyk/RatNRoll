#include "UnlockGraveyardLayer.h"

#include "FacebookLikeNode.h"

#include "Menu/MenuLabel.h"
#include "Base/MipmapSprite.h"

USING_NS_CC;

UnlockGraveyardLayer::UnlockGraveyardLayer()
    : menuHelper({{{0.5, 0.78},
                   0.15,
                   "play",
                   {[](Node *node) {},
                    [](Node *node) { static_cast<MenuItemButton *>(node)->setEnabled(false); }}}},
                 std::bind(&UnlockGraveyardLayer::itemClicked, this, std::placeholders::_1))
{
}

bool UnlockGraveyardLayer::init(bool unlockingEnabled)
{
    if (!Layer::init()) {
        return false;
    }

    setCascadeOpacityEnabled(true);

    addChild(menuHelper.getMenu());

    if (!unlockingEnabled) {
        menuHelper.runActionFor(0, 1);
    }

    addChild(MenuLabel::create("SCORE 10000 POINTS\n"
                               "TO UNLOCK THIS LEVEL",
                               {0.5, 0.9}, 0.03));

    addChild(MenuLabel::create("AND GET A NEW ITEM", {0.5, 0.65}, 0.03));

    Sprite *helmetSprite = MipmapSprite::create("items/shield01.png");
    MenuHelper::positionNode(*helmetSprite, {0.5, 0.57}, 0.075);
    addChild(helmetSprite);
    auto scaleAction = ScaleBy::create(0.5, 1.5);
    helmetSprite->runAction(
        RepeatForever::create(Sequence::create(scaleAction, scaleAction->reverse(), nullptr)));

    addChild(MenuLabel::create("HELMET\n\n"
                               "PROTECTS YOU AGAINST SKULLS",
                               {0.5, 0.47}, 0.03));

    addChild(MenuLabel::create("HAVING FUN? LIKE US!", {0.5, 0.35}, 0.02));

    facebookLikeNode = FacebookLikeNode::create("1775466602680654", {0.5, 0.31}, 0);
    addChild(facebookLikeNode);

    return true;
}

UnlockGraveyardLayer *UnlockGraveyardLayer::create(bool unlockingEnabled)
{
    UnlockGraveyardLayer *ret = new (std::nothrow) UnlockGraveyardLayer();
    if (ret && ret->init(unlockingEnabled)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }

    return ret;
}

void UnlockGraveyardLayer::itemClicked(int itemIndex)
{
    if (itemIndex == 0 && likingCompletedCallback) {
        removeChild(facebookLikeNode);
        likingCompletedCallback();
    }
}
