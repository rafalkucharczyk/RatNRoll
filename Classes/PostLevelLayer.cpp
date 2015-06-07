#include "PostLevelLayer.h"

#include "BackgroundLayer.h"

USING_NS_CC;

Scene *PostLevelLayer::createScene()
{
    auto scene = Scene::create();

    auto layer = PostLevelLayer::create();
    scene->addChild(layer);

    return scene;
}

bool PostLevelLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    MenuItemFont::setFontSize(60);
    auto restartItem =
        MenuItemFont::create("Restart", CC_CALLBACK_1(PostLevelLayer::restartCallback, this));
    restartItem->setPosition(
        Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));

    auto menu = Menu::create(restartItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    return true;
}

void PostLevelLayer::restartCallback(cocos2d::Ref *sender)
{
    if (restartLevelCallback) {
        restartLevelCallback();
    }
}