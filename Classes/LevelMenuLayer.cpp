#include "LevelMenuLayer.h"

USING_NS_CC;

bool LevelMenuLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
                                           CC_CALLBACK_1(LevelMenuLayer::menuCloseCallback, this));
    closeItem->setPosition(
        Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
             origin.y + closeItem->getContentSize().height / 2));

    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    return true;
}

void LevelMenuLayer::menuCloseCallback(Ref *pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    MessageBox("You pressed the close button. Windows Store Apps do not "
               "implement a close button.",
               "Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
