#ifndef __MENU_HELPER_H__
#define __MENU_HELPER_H__

#include "cocos2d.h"

#include <list>
#include <string>

class MenuHelper
{
  public:
    MenuHelper(std::list<std::string> menuStrings, std::function<void(int)> callback);

    cocos2d::Menu *getMenu();

  private:
    cocos2d::MenuItemLabel *createMenuItemLabel(const std::string &text);

    void menuItemLabelCallback(cocos2d::Ref *sender);

  private:
    std::list<std::string> menuStrings;

    cocos2d::Vector<cocos2d::MenuItem *> menuItems;

    std::function<void(int)> menuItemClickedCallback;
};

#endif // __MENU_HELPER_H__
