#include "InitialLayer.h"

USING_NS_CC;

InitialLayer::InitialLayer() : menuStrings{"Tutorial", "Level 1"} {}

bool InitialLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    for (const std::string &s : menuStrings) {
        menuItems.pushBack(createMenuItemLabel(s));
    }

    positionMenuItems();

    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    addChild(menu);

    return true;
}

MenuItemLabel *InitialLayer::createMenuItemLabel(const std::string &text)
{
    auto label = Label::createWithSystemFont(text, "Marker Felt", 60);
    label->setColor(Color3B::BLACK);
    auto item =
        MenuItemLabel::create(label, CC_CALLBACK_1(InitialLayer::menuItemLabelCallback, this));

    return item;
}

void InitialLayer::positionMenuItems()
{
    float itemHeight = menuItems.front()->getContentSize().height;

    float totalHeight =
        itemHeight * menuItems.size() + itemHeight / 2 * (menuItems.size() - 1); // spacing

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    float y = origin.y + visibleSize.height / 2 + totalHeight / 2;
    for (MenuItem *item : menuItems) {
        item->setPosition(origin.x + visibleSize.width / 2, y);

        y -= 1.5 * itemHeight;
    }
}

void InitialLayer::menuItemLabelCallback(cocos2d::Ref *sender)
{
    if (menuItemClickedCallback) {
        int itemIndex =
            std::distance(menuItems.begin(), menuItems.find(dynamic_cast<MenuItem *>(sender)));
        menuItemClickedCallback(itemIndex);
    }
}
