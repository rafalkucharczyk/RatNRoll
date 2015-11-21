#include "LevelSelectionLayer.h"

USING_NS_CC;

LevelSelectionLayer::LevelSelectionLayer(bool shakeTutorialButton)
    : menuHelper({{{0.3 + 0.01, 0.7 + 0.01},
                   0.2,
                   "tutorial",
                   {[shakeTutorialButton](Node *node) {
                       if (shakeTutorialButton) {
                           auto rotateAction = RotateBy::create(0.5, 10);
                           node->runAction(RepeatForever::create(
                               Sequence::create(rotateAction, rotateAction->reverse(), nullptr)));
                       }
                   }}},
                  {{0.7 + 0.01, 0.7 - 0.01}, 0.2, "cheese_factory"},
                  {{0.3 - 0.02, 0.4 + 0.02}, 0.2, "graveyard"},
                  {{0.7 - 0.01, 0.4 - 0.01}, 0.2, "magic_hall"},
                  {{0.5, 0.2}, 0.1, "back"}},
                 std::bind(&LevelSelectionLayer::itemClicked, this, std::placeholders::_1))
{
}

bool LevelSelectionLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    addChild(menuHelper.getMenu());

    return true;
}

LevelSelectionLayer *LevelSelectionLayer::create(bool shakeTutorialButton)
{
    LevelSelectionLayer *ret = new (std::nothrow) LevelSelectionLayer(shakeTutorialButton);
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    } else {
        delete ret;
        ret = nullptr;
        return nullptr;
    }
}
