#ifndef __FACEBOOK_LIKE_NODE_H__
#define __FACEBOOK_LIKE_NODE_H__

#include "cocos2d.h"

#include "FacebookHelper.h"

class FacebookLikeNode : public cocos2d::Node
{
  public:
    // position - (x, y), 0 (left) <= x <= 1 (right), 0 (bottom) <= y <= 1 (top)
    // width - 0 < width <= 1
    static FacebookLikeNode *create(const std::string &objectId, const cocos2d::Vec2 &position,
                                    float width, std::function<void()> likedCallback);
    ~FacebookLikeNode();

  private:
    void addFacebookLoginControl(const std::string &objectId, const cocos2d::Vec2 &position,
                                 float width);

    std::tuple<float, float, float> getViewPositionAndWidth(const cocos2d::Vec2 &position,
                                                            float width);

    FacebookHelper facebookHelper;
    std::function<void()> likedCallback;
};

#endif // __FACEBOOK_NODE_H__
