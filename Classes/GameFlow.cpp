#include "GameFlow.h"

#include "BackgroundLayer.h"
#include "LevelMenuLayer.h"
#include "LevelLayer.h"
#include "PostLevelLayer.h"

USING_NS_CC;

Scene *GameFlow::createLevelScene()
{
    auto scene = Scene::create();

    auto backgroundLayer = BackgroundLayer::create("cheese01.png", "background01.png");
    scene->addChild(backgroundLayer);

    auto levelMenuLayer = LevelMenuLayer::create();
    scene->addChild(levelMenuLayer);

    auto levelLayer = LevelLayer::create();
    scene->addChild(levelLayer);

    levelLayer->setGameFinishedCallback(&GameFlow::switchToPostLevelScene);

    return scene;
}

void GameFlow::switchToLevelScene()
{
    auto scene = createLevelScene();
    Director::getInstance()->replaceScene(scene);
}

void GameFlow::switchToPostLevelScene()
{
    auto scene = Scene::create();

    auto backgroundLayer = BackgroundLayer::create("cheese01.png", "background01.png");
    scene->addChild(backgroundLayer);

    auto postLevelLayer = PostLevelLayer::create();
    postLevelLayer->setRestartLevelCallback(GameFlow::switchToLevelScene);
    scene->addChild(postLevelLayer);

    Director::getInstance()->replaceScene(scene);
}