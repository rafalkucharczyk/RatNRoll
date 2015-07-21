#include "MenuHelper.h"

USING_NS_CC;

MenuHelper::MenuHelper(std::list<ButtonConfig> menuButtons, std::function<void(int)> callback)
    : menuButtons(menuButtons), menuItemClickedCallback(callback)
{
}

Menu *MenuHelper::getMenu()
{
    for (const ButtonConfig &config : menuButtons) {
        menuItems.pushBack(createMenuItemButton(config));
    }

    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(0, 0);

    return menu;
}

void MenuHelper::positionNode(cocos2d::Node &node, const cocos2d::Vec2 &position, float size)
{
    Vec2 visibleSize = Director::getInstance()->getVisibleSize();

    node.setPosition(position.x * visibleSize.x, position.y * visibleSize.y);
    node.setScale(size / (node.getBoundingBox().size.height / visibleSize.y));
}

MenuItemButton *MenuHelper::createMenuItemButton(const ButtonConfig &config)
{
    auto item = MenuItemButton::create(config.imagePath,
                                       CC_CALLBACK_1(MenuHelper::menuItemLabelCallback, this));

    positionNode(*item, config.position, config.size);

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
