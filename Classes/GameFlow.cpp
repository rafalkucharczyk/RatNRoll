#include "GameFlow.h"

#include "SonarFrameworks.h"

#include "BackgroundLayer.h"
#include "InitialLayer.h"
#include "LevelMenuLayer.h"
#include "LevelLayer.h"
#include "PostLevelLayer.h"
#include "PauseLayer.h"
#include "SettingsLayer.h"

#include "PermanentStorage.h"

#include "LevelCustomization.h"
#include "InAppPurchaseHelper.h"

USING_NS_CC;

const std::string GameFlow::iapProductId = "com.nowhere.ratnroll.bonusworlds11";

GameFlow *GameFlow::instance = nullptr;

GameFlow::GameFlow() : currentLevelNumber(noLevelNumber)
{
    SonarCocosHelper::GameCenter::registerChallengeCallback(
        std::bind(&GameFlow::startChallenge, this, std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3));
}

GameFlow &GameFlow::getInstance()
{
    if (instance == nullptr) {
        instance = new GameFlow();
    }

    return *instance;
}

Scene *GameFlow::createInitialScene()
{
    auto scene = createSceneObject();

    auto initialLayer = InitialLayer::create();
    initialLayer->setMenuItemClickedCallback(
        std::bind(&GameFlow::handleInitialSceneMenu, this, std::placeholders::_1));
    scene->addChild(initialLayer);

    return scene;
}

void GameFlow::pauseGame()
{
    if (Director::getInstance()->isPaused() || currentLevelNumber == noLevelNumber) {
        return;
    }

    getCurrentLevelLayer().pauseLevel();

    Director::getInstance()->pause();
    auto pauseLayer = PauseLayer::create();
    pauseLayer->setGameResumedCallback(std::bind(&GameFlow::resumeGame, this));
    pauseLayer->setGameQuitCallback(std::bind(&GameFlow::switchToInitialScene, this));

    Scene *runningScene = Director::getInstance()->getRunningScene();

    if (runningScene->getChildByTag(LevelLayer::proxyOverlayLayer)) {
        runningScene->removeChildByTag(LevelLayer::proxyOverlayLayer);
    }

    runningScene->addChild(pauseLayer, 0, pauseLayerTag);
}

bool GameFlow::adsVisible() const
{
    return !InAppPurchaseHelper::isPurchased(GameFlow::iapProductId);
}

void GameFlow::switchToInitialScene()
{
    currentLevelNumber = noLevelNumber;

    Director::getInstance()->resume();

    Director::getInstance()->replaceScene(createInitialScene());
}

void GameFlow::handleInitialSceneMenu(int itemIndex)
{
    if (itemIndex <= 1) {
        switchToLevelScene(itemIndex);
    } else if (itemIndex == 2) {
        switchToSettingsScene();
    } else if (itemIndex == 3) {
        SonarCocosHelper::GameCenter::showLeaderboard();
    }
}

void GameFlow::switchToLevelScene(int levelNumber)
{
    switchToLevelSceneWithChallenge(levelNumber, {});
}

void GameFlow::switchToLevelSceneWithChallenge(int levelNumber, Challenge challenge)
{
    srand(1); // random, but always the same...

    auto scene = createSceneObject();
    BackgroundLayer *backgroundLayer =
        dynamic_cast<BackgroundLayer *>(scene->getChildByTag(backgroundLayerTag));

    auto levelMenuLayer = LevelMenuLayer::create();
    scene->addChild(levelMenuLayer);
    levelMenuLayer->setGamePausedCallback(std::bind(&GameFlow::pauseGame, this));

    auto levelLayer = LevelLayer::create(getLevelCustomization(levelNumber));
    levelLayer->addShadowRat(challenge.playerName, challenge.score);
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
    auto scene = createSceneObject();

    auto postLevelLayer = PostLevelLayer::create();
    postLevelLayer->displayCurrentScore(score);
    postLevelLayer->displayBestScore(updateBestScore(currentLevelNumber, score));
    postLevelLayer->setRestartLevelCallback(
        std::bind(&GameFlow::switchToLevelScene, this, currentLevelNumber));
    postLevelLayer->setGotoMainMenuCallback(std::bind(&GameFlow::switchToInitialScene, this));
    scene->addChild(postLevelLayer);

    currentLevelNumber = noLevelNumber;

    Director::getInstance()->replaceScene(scene);
}

void GameFlow::switchToSettingsScene()
{
    auto scene = createSceneObject();

    auto settingsLayer = SettingsLayer::createNoInit();
    iapHelper.reset(new InAppPurchaseHelper(iapProductId, *settingsLayer));
    settingsLayer->init([this]() { iapHelper->init(); });

    settingsLayer->setGotoMainMenuCallback([this]() {
        iapHelper.reset();
        switchToInitialScene();
    });
    settingsLayer->setPurchaseRequestedCallback([this]() { iapHelper->purchaseProduct(); });
    scene->addChild(settingsLayer);

    Director::getInstance()->replaceScene(scene);
}

void GameFlow::resumeGame()
{
    Director::getInstance()->resume();

    Director::getInstance()->getRunningScene()->removeChildByTag(pauseLayerTag);

    getCurrentLevelLayer().resumeLevel();
}

void GameFlow::startChallenge(std::string playerName, int score, std::string leaderboardId)
{
    int levelNumber = std::stoi(leaderboardId.substr(leaderboardId.size() - 1, 1));

    Director::getInstance()->resume();

    switchToLevelSceneWithChallenge(levelNumber, Challenge(playerName, score));
}

Scene *GameFlow::createSceneObject()
{
    Scene *scene = Scene::create();

    auto backgroundLayer = BackgroundLayer::create("cheese01.png", "background01.png");
    scene->addChild(backgroundLayer, 0, backgroundLayerTag);

    return scene;
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
    SonarCocosHelper::GameCenter::submitScore(score, "ratnroll_leaderboard_" +
                                                         std::to_string(levelNumber));

    PermanentStorage gameStorage;

    int currentBestScore = gameStorage.getBestScore(levelNumber);

    if (score > currentBestScore) {
        gameStorage.setBestScore(levelNumber, score);

        return score;
    }

    return currentBestScore;
}

LevelLayer &GameFlow::getCurrentLevelLayer()
{
    LevelLayer *levelLayer =
        Director::getInstance()->getRunningScene()->getChildByName<LevelLayer *>(LevelLayer::name);
    assert(levelLayer);

    return *levelLayer;
}
