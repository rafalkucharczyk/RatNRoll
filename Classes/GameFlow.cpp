#include "GameFlow.h"

#include "BackgroundLayer.h"
#include "InitialLayer.h"
#include "LevelMenuLayer.h"
#include "LevelLayer.h"
#include "PostLevelLayer.h"

#include "PermanentStorage.h"

#include "LevelCustomization.h"

USING_NS_CC;

GameFlow *GameFlow::instance = nullptr;

GameFlow &GameFlow::getInstance()
{
    if (instance == nullptr) {
        instance = new GameFlow();
    }

    return *instance;
}

Scene *GameFlow::createInitialScene()
{
    auto scene = Scene::create();

    auto backgroundLayer = BackgroundLayer::create("cheese01.png", "background01.png");
    backgroundLayer->setSpeed(5);
    scene->addChild(backgroundLayer);

    auto initialLayer = InitialLayer::create();
    initialLayer->setMenuItemClickedCallback(
        std::bind(&GameFlow::switchToLevelScene, this, std::placeholders::_1));
    scene->addChild(initialLayer);

    return scene;
}

void GameFlow::switchToInitialScene()
{
    Director::getInstance()->replaceScene(createInitialScene());
}

void GameFlow::switchToLevelScene(int levelNumber)
{
    srand(1); // random, but always the same...

    auto scene = Scene::create();

    auto backgroundLayer = BackgroundLayer::create("cheese01.png", "background01.png");
    scene->addChild(backgroundLayer);

    auto levelMenuLayer = LevelMenuLayer::create();
    scene->addChild(levelMenuLayer);

    auto levelLayer = LevelLayer::create(getLevelCustomization(levelNumber));
    currentLevelNumber = levelNumber;
    scene->addChild(levelLayer);

    levelLayer->setGameFinishedCallback(
        std::bind(&GameFlow::switchToPostLevelScene, this, std::placeholders::_1));
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
    postLevelLayer->setRestartLevelCallback(
        std::bind(&GameFlow::switchToLevelScene, this, currentLevelNumber));
    postLevelLayer->setGotoMainMenuCallback(std::bind(&GameFlow::switchToInitialScene, this));
    scene->addChild(postLevelLayer);

    Director::getInstance()->replaceScene(scene);
}

LevelCustomization *GameFlow::getLevelCustomization(int levelNumber) const
{
    switch (levelNumber) {
    case 0:
        return new LevelTutorial();
        break;

    case 1:
        return new Level01();
        break;

    default:
        assert(false);
        return new LevelTutorial();
        break;
    }
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