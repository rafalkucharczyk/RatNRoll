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

    // ---
    auto restartLabel = Label::createWithSystemFont("Restart", "Marker Felt", 60);
    restartLabel->setColor(Color3B::BLACK);
    auto restartItem =
        MenuItemLabel::create(restartLabel, CC_CALLBACK_1(PostLevelLayer::restartCallback, this));
    restartItem->setPosition(
        Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));

    auto menu = Menu::create(restartItem, NULL);
    menu->setPosition(Vec2::ZERO);
    addChild(menu, 1);

    // ---
    bestScoreLabel = Label::createWithSystemFont("0", "Marker Felt", 40);
    bestScoreLabel->setColor(Color3B::BLACK);
    bestScoreLabel->setPosition(
        restartItem->getPosition() -
        Vec2(0, restartItem->getContentSize().height + bestScoreLabel->getContentSize().height));

    addChild(bestScoreLabel, 1);

    return true;
}

void PostLevelLayer::restartCallback(cocos2d::Ref *sender)
{
    if (restartLevelCallback) {
        restartLevelCallback();
    }
}

void PostLevelLayer::displayBestScore(int score)
{
    bestScoreLabel->setString("Best score: " + std::to_string(score));
}