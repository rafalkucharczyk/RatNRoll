#include "TransitionSlideChildren.h"

USING_NS_CC;

TransitionSlideChildren *TransitionSlideChildren::create(float t, Scene *scene)
{
    TransitionSlideChildren *transition = new (std::nothrow) TransitionSlideChildren();
    transition->initWithDuration(t, scene);
    transition->autorelease();

    return transition;
}

void TransitionSlideChildren::onEnter()
{
    TransitionScene::onEnter();

    _inScene->setVisible(false);

    float t = _duration / 2;

    _outScene->runAction(Sequence::create(CallFuncN::create([this, t](Node *node) {
                                              node->enumerateChildren("//", [this, t](Node *child) {
                                                  child->runAction(MoveTo::create(
                                                      t, this->getAnimationPoint(*child)));
                                                  return false;
                                              });
                                          }),
                                          DelayTime::create(t), Hide::create(), nullptr));

    _inScene->runAction(
        Sequence::create(CallFuncN::create([this, t](Node *node) {
                             node->enumerateChildren("//", [this, t](Node *child) {
                                 CCLOG("child=%p", child);
                                 Vec2 p = child->getPosition();
                                 child->setPosition(this->getAnimationPoint(*child));
                                 child->runAction(Sequence::create(DelayTime::create(t),
                                                                   MoveTo::create(t, p), nullptr));
                                 return false;
                             });
                         }),
                         DelayTime::create(t),
                         CallFunc::create(CC_CALLBACK_0(TransitionScene::finish, this)), nullptr));
}

Vec2 TransitionSlideChildren::getAnimationPoint(const Node &node)
{
    Size s = Director::getInstance()->getVisibleSize();
    Vec2 p = node.getPosition();
    Rect b = node.boundingBox();

    float x = 0, y = 0;

    x = p.x > (s.width / 2) ? s.width + b.size.width : -b.size.width;
    y = p.y > (s.height / 2) ? s.height + b.size.height : -b.size.height;

    float dx = fabs(p.x - x);
    float dy = fabs(p.y - y);

    return Vec2(dx <= dy ? x : p.x, dy < dx ? y : p.y);
}