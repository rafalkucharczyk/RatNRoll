#ifndef __MENU_LABEL_H__
#define __MENU_LABEL_H__

#include "cocos2d.h"

#include <string>

class MenuLabel : public cocos2d::Node
{
  public:
    MenuLabel();

    // position - (x, y), 0 (left) <= x <= 1 (right), 0 (bottom) <= y <= 1 (top)
    // lineSize - size of the single line (relative to height of the screen), 0 <= size <= 1
    static MenuLabel *create(const std::string &text, const cocos2d::Vec2 &position,
                             float lineSize);

    float getTotalSize() const;

  private:
    void addLabel(const std::string &text, const cocos2d::Vec2 &position, float size);

    cocos2d::Label *label;
};

#endif // __MENU_LABEL_H__
