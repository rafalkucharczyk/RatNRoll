#include "FacebookLikeNode.h"

#include "MenuHelper.h"

USING_NS_CC;

FacebookLikeNode::~FacebookLikeNode()
{
    facebookHelper.hideLoginControl();
    facebookHelper.hideLikeControl();
}

FacebookLikeNode *FacebookLikeNode::create(const std::string &objectId, const Vec2 &position,
                                           float width, std::function<void()> likedCallback)
{
    FacebookLikeNode *ret = new (std::nothrow) FacebookLikeNode();
    ret->likedCallback = likedCallback;

    if (ret && ret->init()) {
        ret->autorelease();

        ret->addFacebookLoginControl(objectId, position, width);
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

void FacebookLikeNode::addFacebookLoginControl(const std::string &objectId, const Vec2 &position,
                                               float width)
{
    float x = 0, y = 0, w = 0;
    std::tie(x, y, w) = getViewPositionAndWidth(position, width);

    facebookHelper.setLikeCallback([this](bool liked) {
        if (liked) {
            facebookHelper.hideLikeControl();
            if (likedCallback) {
                // unregister callback as since FacebookLikeNode (and its facebookHelper) will
                // be destroyed in likedCallback
                facebookHelper.setLikeCallback(nullptr);
                likedCallback();
            }
        }
    });

    facebookHelper.setLoginCallback([this, x, y, w, objectId](bool loggedIn) {
        if (loggedIn) {
            facebookHelper.hideLoginControl();
            facebookHelper.showLikeControl(x, y, w, objectId);
        }
    });

    facebookHelper.showLoginControl(x, y, w);
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
