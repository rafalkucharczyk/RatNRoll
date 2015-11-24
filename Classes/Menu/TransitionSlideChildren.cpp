#include "TransitionSlideChildren.h"

#include "MenuItemButton.h"
#include "DigitsPanel.h"
#include "MenuLabel.h"

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
                                              this->listChildren(node, [this, t](Node *child) {
                                                  child->runAction(MoveTo::create(
                                                      t, this->getAnimationPoint(*child)));
                                              });
                                          }),
                                          DelayTime::create(t), Hide::create(), nullptr));

    _inScene->runAction(
        Sequence::create(CallFuncN::create([this, t](Node *node) {
                             this->listChildren(node, [this, t](Node *child) {
                                 Vec2 p = child->getPosition();
                                 child->setPosition(this->getAnimationPoint(*child));
                                 child->runAction(Sequence::create(DelayTime::create(t),
                                                                   MoveTo::create(t, p), nullptr));
                             });
                         }),
                         DelayTime::create(t),
                         CallFunc::create(CC_CALLBACK_0(TransitionScene::finish, this)), nullptr));
}

void TransitionSlideChildren::listChildren(Node *node, std::function<void(Node *node)> callback)
{
    auto filter = [](Node *_node) {
        return dynamic_cast<MenuItemButton *>(_node) || dynamic_cast<DigitsPanel *>(_node) ||
        dynamic_cast<Label*>(_node);
    };

    for (Node *child : node->getChildren()) {
        if (filter(child)) {
            callback(child);
        }
        listChildren(child, callback);
    }
}

Vec2 TransitionSlideChildren::getAnimationPoint(const Node &node)
{
    Size s = Director::getInstance()->getVisibleSize();
    Vec2 p = node.getPosition();
    Size b = node.boundingBox().size;

    float x = 0, y = 0;

    x = p.x > (s.width / 2) ? s.width + b.width : -b.width;
    y = p.y > (s.height / 2) ? s.height + b.height : -b.height;

    float dx = fabs(p.x - x);
    float dy = fabs(p.y - y);

    return Vec2(dx <= dy ? x : p.x, dy < dx ? y : p.y);
}