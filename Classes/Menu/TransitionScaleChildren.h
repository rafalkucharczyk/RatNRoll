#ifndef __TRANSITION_SCALE_CHILDREN_H__
#define __TRANSITION_SCALE_CHILDREN_H__

#include <cocos2d.h>

class TransitionScaleChildren : public cocos2d::TransitionScene
{
  public:
    static TransitionScaleChildren *create(float t, Scene *scene);
    void onEnter();

  private:
    void listChildren(Node *node, std::function<void(Node *node)> callback);
};

#endif // __TRANSITION_SCALE_CHILDREN_H__