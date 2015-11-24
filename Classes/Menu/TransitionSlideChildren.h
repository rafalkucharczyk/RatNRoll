#ifndef __TRANSITION_SLIDE_CHILDREN_H__
#define __TRANSITION_SLIDE_CHILDREN_H__

#include <cocos2d.h>

class TransitionSlideChildren : public cocos2d::TransitionScene
{
  public:
    static TransitionSlideChildren *create(float t, Scene *scene);
    void onEnter();

  private:
    void listChildren(Node *node, std::function<void(Node *node)> callback);

    cocos2d::Vec2 getAnimationPoint(const Node &node);
};

#endif // __TRANSITION_SLIDE_CHILDREN_H__