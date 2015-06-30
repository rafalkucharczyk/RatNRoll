#include "MenuHelper.h"

USING_NS_CC;

MenuHelper::MenuHelper(std::list<std::string> menuStrings, std::function<void(int)> callback)
    : menuStrings(menuStrings), menuItemClickedCallback(callback)
{
}

Menu *MenuHelper::getMenu()
{
    for (const std::string &s : menuStrings) {
        menuItems.pushBack(createMenuItemLabel(s));
    }

    auto menu = Menu::createWithArray(menuItems);
    menu->alignItemsVerticallyWithPadding(50);

    return menu;
}

MenuItemLabel *MenuHelper::createMenuItemLabel(const std::string &text)
{
    auto label = Label::createWithSystemFont(text, "Marker Felt", 60);
    label->setColor(Color3B::BLACK);
    auto item =
        MenuItemLabel::create(label, CC_CALLBACK_1(MenuHelper::menuItemLabelCallback, this));

    return item;
}

void MenuHelper::menuItemLabelCallback(cocos2d::Ref *sender)
{
    if (menuItemClickedCallback) {
        int itemIndex =
            std::distance(menuItems.begin(), menuItems.find(dynamic_cast<MenuItem *>(sender)));
        menuItemClickedCallback(itemIndex);
    }
}
