#include "FacebookLikeNode.h"

#include "MenuHelper.h"

USING_NS_CC;

FacebookLikeNode::~FacebookLikeNode() { facebookHelper.hideLikeControl(); }

FacebookLikeNode *FacebookLikeNode::create(const std::string &objectId, const Vec2 &position,
                                           float width)
{
    FacebookLikeNode *ret = new (std::nothrow) FacebookLikeNode();

    if (ret && ret->init()) {
        ret->autorelease();

        ret->addFacebookLikeControl(objectId, position, width);
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

void FacebookLikeNode::addFacebookLikeControl(const std::string &objectId, const Vec2 &position,
                                              float width)
{
    float x = 0, y = 0, w = 0;
    std::tie(x, y, w) = getViewPositionAndWidth(position, width);

    facebookHelper.showLikeControl(x, y, w, objectId);
}

std::tuple<float, float, float>
FacebookLikeNode::getViewPositionAndWidth(const cocos2d::Vec2 &position, float width)
{
    GLViewImpl *glView = dynamic_cast<GLViewImpl *>(Director::getInstance()->getOpenGLView());
    assert(glView);

    Vec2 p = Vec2(position.x, 1 - position.y) / glView->getContentScaleFactor();

    MenuHelper::positionNode(*this, p, 1);

    float w =
        width / glView->getContentScaleFactor() * Director::getInstance()->getVisibleSize().width;

    return std::make_tuple(getPosition().x, getPosition().y, w);
}
