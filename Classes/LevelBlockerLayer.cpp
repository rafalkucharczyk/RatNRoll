#include "LevelBlockerLayer.h"

USING_NS_CC;

LevelBlockerLayer::LevelBlockerLayer(LevelLayer &levelLayer)
    : menuHelper({{{0.5, 0.325}, 0.1, "back"}},
                 std::bind(&LevelBlockerLayer::itemClicked, this, std::placeholders::_1)),
      levelLayer(levelLayer)
{
}

bool LevelBlockerLayer::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 232))) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    setCascadeOpacityEnabled(true);

    return true;
}

LevelBlockerLayer *LevelBlockerLayer::create(LevelLayer &levelLayer)
{
    LevelBlockerLayer *ret = new (std::nothrow) LevelBlockerLayer(levelLayer);
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }

    return ret;
}

void LevelBlockerLayer::itemClicked(int itemIndex)
{
    if (itemIndex == 0 && gameGotoMenuCallback) {
        gameGotoMenuCallback();
    }
}

void LevelBlockerLayer::unblock()
{
    runAction(Sequence::create(FadeOut::create(0.5), CallFuncN::create([this](Node *node) {
                                   node->removeFromParentAndCleanup(true);
                                   levelLayer.resumeLevel();
                               }),
                               nullptr));
}

void LevelBlockerLayer::onEnterTransitionDidFinish()
{
    levelLayer.runAction(Sequence::create(
        DelayTime::create(0.5), CallFunc::create([this]() { levelLayer.pauseLevel(); }), nullptr));
}
