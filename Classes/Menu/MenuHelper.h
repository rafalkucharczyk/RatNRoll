#ifndef __MENU_HELPER_H__
#define __MENU_HELPER_H__

#include "cocos2d.h"

#include "MenuItemButton.h"

#include <vector>
#include <string>

struct ButtonConfig {
    typedef std::function<void(cocos2d::Node *)> Action;
    ButtonConfig(const cocos2d::Vec2 &position, float size, const std::string &imagePath,
                 std::vector<Action> customActions = {})
        : position(position), size(size), imagePath(imagePath), customActions(customActions)
    {
    }

    cocos2d::Vec2 position; // x, y, 0.0 - 1.0 of screen width, height
    float size;             // 0.0 - 1.0 of screen height
    std::string imagePath;
    std::vector<Action> customActions;
};

class MenuHelper
{
  public:
    MenuHelper(std::vector<ButtonConfig> menuButtons, std::function<void(int)> callback);

    cocos2d::Menu *getMenu();

    void runActionFor(int itemIndex, int actionIndex);

  public:
    // some default actions
    static ButtonConfig::Action removeTransparency();
    static ButtonConfig::Action replaceImage(std::function<bool()> selector,
                                             const std::string &firstImage,
                                             const std::string &secondImage);

  public:
    static void positionNode(cocos2d::Node &node, const cocos2d::Vec2 &position, float size);
    static float getContentScaleFactor();

  private:
    MenuItemButton *createMenuItemButton(const ButtonConfig &config);

    void menuItemLabelCallback(cocos2d::Ref *sender);

  private:
    std::vector<ButtonConfig> menuButtons;

    cocos2d::Vector<cocos2d::MenuItem *> menuItems;

    std::function<void(int)> menuItemClickedCallback;
};

#endif // __MENU_HELPER_H__
