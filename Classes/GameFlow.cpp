#include "GameFlow.h"

#include "BackgroundLayer.h"
#include "InitialLayer.h"
#include "LevelMenuLayer.h"
#include "LevelLayer.h"
#include "PostLevelLayer.h"

#include "PermanentStorage.h"

USING_NS_CC;

Scene *GameFlow::createInitialScene()
{
    auto scene = Scene::create();

    auto backgroundLayer = BackgroundLayer::create("cheese01.png", "background01.png");
    scene->addChild(backgroundLayer);

    auto initialLayer = InitialLayer::create();
    initialLayer->setMenuItemClickedCallback(
        std::bind(&GameFlow::switchToLevelScene, std::placeholders::_1));
    scene->addChild(initialLayer);

    return scene;
}

void GameFlow::switchToLevelScene(int levelNumber)
{
    srand(1); // random, but always the same...

    auto scene = Scene::create();

    auto backgroundLayer = BackgroundLayer::create("cheese01.png", "background01.png");
    scene->addChild(backgroundLayer);

    auto levelMenuLayer = LevelMenuLayer::create();
    scene->addChild(levelMenuLayer);

    auto levelLayer = LevelLayer::create();
    scene->addChild(levelLayer);

    levelLayer->setGameFinishedCallback(&GameFlow::switchToPostLevelScene);
    levelLayer->setBackgroundSpeedFunction(
        std::bind(&BackgroundLayer::setSpeed, backgroundLayer, std::placeholders::_1));

    Director::getInstance()->replaceScene(scene);
}

void GameFlow::switchToPostLevelScene(int score)
{
    auto scene = Scene::create();

    auto backgroundLayer = BackgroundLayer::create("cheese01.png", "background01.png");
    backgroundLayer->setSpeed(5);
    scene->addChild(backgroundLayer);

    auto postLevelLayer = PostLevelLayer::create();
    postLevelLayer->displayBestScore(updateBestScore(score));
    postLevelLayer->setRestartLevelCallback(std::bind(&GameFlow::switchToLevelScene, 1));
    scene->addChild(postLevelLayer);

    Director::getInstance()->replaceScene(scene);
}

int GameFlow::updateBestScore(int score)
{
    PermanentStorage gameStorage;

    int currentBestScore = gameStorage.getBestScore();

    if (score > currentBestScore) {
        gameStorage.setBestScore(score);

        return score;
    }

    return currentBestScore;
}