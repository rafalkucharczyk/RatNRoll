#include "LevelSelectionLayer.h"

USING_NS_CC;

namespace
{

void runSwingAction(Node *node)
{
    node->runAction(RepeatForever::create(Sequence::create(
        RotateBy::create(0.5, 10), RotateBy::create(1, -20), RotateBy::create(0.5, 10), nullptr)));
}
};

LevelSelectionLayer::LevelSelectionLayer(bool swingTutorialButton, bool swingCheeseFactoryButton)
    : menuHelper({{{0.3 + 0.01, 0.7 + 0.01},
                   0.2,
                   "tutorial",
                   {[swingTutorialButton](Node *node) {
                       if (swingTutorialButton) {
                           runSwingAction(node);
                       }
                   }}},
                  {{0.7 + 0.01, 0.7 - 0.01},
                   0.2,
                   "cheese_factory",
                   {[swingCheeseFactoryButton](Node *node) {
                       if (swingCheeseFactoryButton) {
                           runSwingAction(node);
                       }
                   }}},
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

LevelSelectionLayer *LevelSelectionLayer::create(bool swingTutorialButton,
                                                 bool swingCheeseFactoryButton)
{
    LevelSelectionLayer *ret =
        new (std::nothrow) LevelSelectionLayer(swingTutorialButton, swingCheeseFactoryButton);
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    } else {
        delete ret;
        ret = nullptr;
        return nullptr;
    }
}
