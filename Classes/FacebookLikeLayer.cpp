#include "FacebookLikeLayer.h"

USING_NS_CC;

FacebookLikeLayer::FacebookLikeLayer(const cocos2d::Vec2 &buttonPosition, float buttonSize)
    : menuHelper({{buttonPosition, buttonSize, "play"}},
                 std::bind(&FacebookLikeLayer::menuItemClicked, this, std::placeholders::_1))
{
}

bool FacebookLikeLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    setCascadeOpacityEnabled(true);

    return true;
}

FacebookLikeLayer *FacebookLikeLayer::create(const cocos2d::Vec2 &buttonPosition, float buttonSize)
{
    FacebookLikeLayer *ret = new (std::nothrow) FacebookLikeLayer(buttonPosition, buttonSize);
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }

    return ret;
}

void FacebookLikeLayer::menuItemClicked(int itemIndex)
{
    if (itemIndex == 0) {
        likingCompletedCallback();
    }
}
