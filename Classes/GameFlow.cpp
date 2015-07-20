#include "GameFlow.h"

#include "SonarFrameworks.h"

#include "BackgroundLayer.h"
#include "InitialLayer.h"
#include "LevelMenuLayer.h"
#include "LevelLayer.h"
#include "PostLevelLayer.h"
#include "PauseLayer.h"

#include "PermanentStorage.h"

#include "LevelCustomization.h"

USING_NS_CC;

GameFlow *GameFlow::instance = nullptr;

GameFlow::GameFlow() : currentLevelNumber(0) {}

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
        std::bind(&GameFlow::handleInitialSceneMenu, this, std::placeholders::_1));
    scene->addChild(initialLayer);

    return scene;
}

void GameFlow::pauseGame()
{
    if (Director::getInstance()->isPaused()) {
        return;
    }

    Director::getInstance()->pause();
    auto pauseLayer = PauseLayer::create();
    pauseLayer->setGameResumedCallback(std::bind(&GameFlow::resumeGame, this));
    pauseLayer->setGameQuitCallback(std::bind(&GameFlow::switchToInitialScene, this));

    Director::getInstance()->getRunningScene()->addChild(pauseLayer, 0, pauseLayerTag);
}

void GameFlow::switchToInitialScene()
{
    Director::getInstance()->resume();
    Director::getInstance()->replaceScene(createInitialScene());
}

void GameFlow::handleInitialSceneMenu(int itemIndex)
{
    if (itemIndex <= 1) {
        switchToLevelScene(itemIndex);
    } else if (itemIndex == 3) {
        SonarCocosHelper::GameCenter::showLeaderboard();
    }
}

void GameFlow::switchToLevelScene(int levelNumber)
{
    srand(1); // random, but always the same...

    auto scene = Scene::create();

    auto backgroundLayer = BackgroundLayer::create("cheese01.png", "background01.png");
    scene->addChild(backgroundLayer);

    auto levelMenuLayer = LevelMenuLayer::create();
    scene->addChild(levelMenuLayer);
    levelMenuLayer->setGamePausedCallback(std::bind(&GameFlow::pauseGame, this));

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
    postLevelLayer->displayCurrentScore(score);
    postLevelLayer->displayBestScore(updateBestScore(currentLevelNumber, score));
    postLevelLayer->setRestartLevelCallback(
        std::bind(&GameFlow::switchToLevelScene, this, currentLevelNumber));
    postLevelLayer->setGotoMainMenuCallback(std::bind(&GameFlow::switchToInitialScene, this));
    scene->addChild(postLevelLayer);

    Director::getInstance()->replaceScene(scene);
}

void GameFlow::resumeGame()
{
    Director::getInstance()->resume();

    Director::getInstance()->getRunningScene()->removeChildByTag(pauseLayerTag);
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

int GameFlow::updateBestScore(int levelNumber, int score)
{
    PermanentStorage gameStorage;

    int currentBestScore = gameStorage.getBestScore(levelNumber);

    if (score > currentBestScore) {
        gameStorage.setBestScore(levelNumber, score);

        SonarCocosHelper::GameCenter::submitScore(score, "ratnroll_leaderboard_" +
                                                             std::to_string(levelNumber));

        return score;
    }

    return currentBestScore;
}
