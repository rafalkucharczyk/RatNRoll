#include "FacebookLikeLayer.h"

#include "FacebookLikeNode.h"

USING_NS_CC;

FacebookLikeLayer::FacebookLikeLayer(const cocos2d::Vec2 &buttonPosition, float buttonSize)
    : buttonPosition(buttonPosition), buttonSize(buttonSize)
{
}

bool FacebookLikeLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    setCascadeOpacityEnabled(true);

    auto facebookNode =
        FacebookLikeNode::create("1775466602680654", buttonPosition, buttonSize, [this]() {
            if (likingCompletedCallback) {
                likingCompletedCallback();
            }
        });
    addChild(facebookNode);

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
