#ifndef __INITIAL_LAYER_H__
#define __INITIAL_LAYER_H__

#include "cocos2d.h"

class InitialLayer : public cocos2d::Layer
{
  public:
    InitialLayer();
    virtual bool init();

    CREATE_FUNC(InitialLayer);

    void setMenuItemClickedCallback(std::function<void(int)> callback)
    {
        menuItemClickedCallback = callback;
    }

  private:
    cocos2d::MenuItemLabel *createMenuItemLabel(const std::string &text);
    void positionMenuItems();

    void menuItemLabelCallback(cocos2d::Ref *sender);

  private:
    std::list<std::string> menuStrings;

    cocos2d::Vector<cocos2d::MenuItem *> menuItems;

    std::function<void(int)> menuItemClickedCallback;
};

#endif // __INITIAL_LAYER_H__
