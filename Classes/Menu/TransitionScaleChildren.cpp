#include "TransitionScaleChildren.h"

#include "MenuItemButton.h"
#include "DigitsPanel.h"
#include "MenuLabel.h"
#include <spine/spine-cocos2dx.h>

USING_NS_CC;

TransitionScaleChildren *TransitionScaleChildren::create(float t, Scene *scene)
{
    TransitionScaleChildren *transition = new (std::nothrow) TransitionScaleChildren();
    transition->initWithDuration(t, scene);
    transition->autorelease();

    return transition;
}

void TransitionScaleChildren::onEnter()
{
    TransitionScene::onEnter();

    _inScene->setVisible(false);

    float t = _duration / 2;

    _outScene->runAction(Sequence::create(CallFuncN::create([this, t](Node *node) {
                                              this->listChildren(node, [this, t](Node *child) {
                                                  child->runAction(ScaleTo::create(t, 0));
                                              });
                                          }),
                                          DelayTime::create(t), Hide::create(), nullptr));

    _inScene->runAction(Sequence::create(
        CallFuncN::create([this, t](Node *node) {
            this->listChildren(node, [this, t](Node *child) {
                float scale = child->getScale();
                child->setScale(0);
                child->runAction(
                    Sequence::create(DelayTime::create(t), ScaleTo::create(t, scale), nullptr));
            });
        }),
        DelayTime::create(t), CallFunc::create(CC_CALLBACK_0(TransitionScene::finish, this)),
        nullptr));
}

void TransitionScaleChildren::listChildren(Node *node, std::function<void(Node *node)> callback)
{
    auto filter = [](Node *_node) {
        return dynamic_cast<MenuItemButton *>(_node) || dynamic_cast<DigitsPanel *>(_node) ||
               dynamic_cast<Label *>(_node) || dynamic_cast<spine::SkeletonAnimation *>(_node) ||
               (dynamic_cast<Sprite *>(_node) &&
                dynamic_cast<Sprite *>(_node)->getTexture()->hasMipmaps());
    };

    for (Node *child : node->getChildren()) {
        if (filter(child)) {
            callback(child);
        } else {
            listChildren(child, callback);
        }
    }
}
