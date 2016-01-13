#include "MenuHelper.h"

USING_NS_CC;

MenuHelper::MenuHelper(std::vector<ButtonConfig> menuButtons, std::function<void(int)> callback)
    : menuButtons(menuButtons), menuItemClickedCallback(callback)
{
}

Menu *MenuHelper::getMenu()
{
    for (const ButtonConfig &config : menuButtons) {
        menuItems.pushBack(createMenuItemButton(config));
        runActionFor(menuItems.size() - 1, 0);
    }

    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(0, 0);

    return menu;
}

void MenuHelper::runActionFor(int itemIndex, int actionIndex)
{
    if (itemIndex < menuButtons.size() &&
        actionIndex < menuButtons[itemIndex].customActions.size()) {
        assert(itemIndex < menuItems.size());

        menuButtons[itemIndex].customActions[actionIndex](menuItems.at(itemIndex));
    }
}

ButtonConfig::Action MenuHelper::removeTransparency()
{
    return [](Node *node) { node->getChildren().front()->getChildren().front()->setOpacity(255); };
}

ButtonConfig::Action MenuHelper::replaceImage(std::function<bool()> selector,
                                              const std::string &firstImage,
                                              const std::string &secondImage)
{
    return [selector, firstImage, secondImage](Node *node) {
        MenuItemSprite *menuItemSprite = dynamic_cast<MenuItemSprite *>(node);

        std::string img = selector() ? firstImage : secondImage;

        auto sprite = MenuItemButton::createSpriteForPath(img);

        menuItemSprite->setNormalImage(sprite);
    };
}

void MenuHelper::positionNode(cocos2d::Node &node, const cocos2d::Vec2 &position, float size)
{
    Vec2 visibleSize = Director::getInstance()->getVisibleSize();

    node.setPosition(position.x * visibleSize.x, position.y * visibleSize.y);
    node.setScale(size / (node.getBoundingBox().size.height / visibleSize.y));
}

float MenuHelper::getContentScaleFactor()
{
    GLViewImpl *glView = dynamic_cast<GLViewImpl *>(Director::getInstance()->getOpenGLView());
    assert(glView);

    return glView->getContentScaleFactor();
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
