#ifndef __MENU_HELPER_H__
#define __MENU_HELPER_H__

#include "cocos2d.h"

#include "MenuItemButton.h"

#include <list>
#include <string>

struct ButtonConfig {
    cocos2d::Vec2 position; // x, y, 0.0 - 1.0 of screen width, height
    float size;             // 0.0 - 1.0 of screen height
    std::string imagePath;
};

class MenuHelper
{
  public:
    MenuHelper(std::list<ButtonConfig> menuButtons, std::function<void(int)> callback);

    cocos2d::Menu *getMenu();

  public:
    static void positionNode(cocos2d::Node &node, const cocos2d::Vec2 &position, float size);

  private:
    MenuItemButton *createMenuItemButton(const ButtonConfig &config);

    void menuItemLabelCallback(cocos2d::Ref *sender);

  private:
    std::list<ButtonConfig> menuButtons;

    cocos2d::Vector<cocos2d::MenuItem *> menuItems;

    std::function<void(int)> menuItemClickedCallback;
};

#endif // __MENU_HELPER_H__
