#include "GameFlow.h"

#include "SonarFrameworks.h"

#include "BackgroundLayer.h"
#include "InitialLayer.h"
#include "LevelMenuLayer.h"
#include "LevelLayer.h"
#include "PostLevelLayer.h"
#include "PauseLayer.h"
#include "SettingsLayer.h"
#include "TestLayer.h"

#include "PermanentStorage.h"

#include "LevelCustomization.h"
#include "InAppPurchaseHelper.h"

USING_NS_CC;

const std::string GameFlow::iapProductId = "com.nowhere.ratnroll.bonusworlds11";

GameFlow *GameFlow::instance = nullptr;

GameFlow::GameFlow() : currentLevelNumber(noLevelNumber) { loginToGameCenter(); }

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

bool GameFlow::iapPurchaseCompleted() const
{
    return InAppPurchaseHelper::isPurchased(GameFlow::iapProductId);
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
    } else if (itemIndex == 4) {
        // about
    }
}

void GameFlow::switchToLevelScene(int levelNumber)
{
    switchToLevelSceneWithScores(levelNumber, SonarCocosHelper::GameCenter::getFriendsBestScores(
                                                  getLeaderboardName(levelNumber)));
}

void GameFlow::switchToLevelSceneWithScores(int levelNumber,
                                            SonarCocosHelper::GameCenterPlayersScores scores)
{
    srand(1); // random, but always the same...

    auto scene = createSceneObject();
    BackgroundLayer *backgroundLayer =
        dynamic_cast<BackgroundLayer *>(scene->getChildByTag(backgroundLayerTag));

    auto levelMenuLayer = LevelMenuLayer::create();
    scene->addChild(levelMenuLayer);
    levelMenuLayer->setGamePausedCallback(std::bind(&GameFlow::pauseGame, this));

    auto levelLayer = LevelLayer::create(getLevelCustomization(levelNumber));

    for (const SonarCocosHelper::GameCenterPlayerScore &score : scores) {
        levelLayer->addShadowRat(score.playerName, score.getLowerScoreBound(), score.score);
    }

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
    postLevelLayer->setShareOnFacebookCallback([this, score]() {
        SonarCocosHelper::Facebook::Share(nullptr, nullptr, getSocialShareMessage(score).c_str(),
                                          nullptr, "");
    });
    postLevelLayer->setShareOnTwitterCallback([this, score]() {
        SonarCocosHelper::Twitter::Tweet(getSocialShareMessage(score).c_str(), nullptr, "");
    });
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
    settingsLayer->setPurchaseCompletedCallback([]() { SonarCocosHelper::iAds::hideiAdBanner(); });
    scene->addChild(settingsLayer);

    Director::getInstance()->replaceScene(scene);
}

#ifdef COCOS2D_DEBUG
void GameFlow::switchToTestScene()
{
    auto scene = Scene::create();

    auto layer = TestLayer::create();

    scene->addChild(layer);

    Director::getInstance()->replaceScene(scene);
}
#endif

void GameFlow::resumeGame()
{
    Director::getInstance()->resume();

    Director::getInstance()->getRunningScene()->removeChildByTag(pauseLayerTag);

    getCurrentLevelLayer().resumeLevel();
}

void GameFlow::loginToGameCenter()
{
    SonarCocosHelper::GameCenter::registerChallengeCallback(
        std::bind(&GameFlow::startChallenge, this, std::placeholders::_1));

    SonarCocosHelper::GameCenter::signIn([this]() {
        SonarCocosHelper::GameCenter::getFriendsBestScores(getLeaderboardName(0));
        SonarCocosHelper::GameCenter::getFriendsBestScores(getLeaderboardName(1));
    });
}

void GameFlow::startChallenge(SonarCocosHelper::GameCenterPlayerScore score)
{
    int levelNumber = std::stoi(score.leaderboardId.substr(score.leaderboardId.size() - 1, 1));

    Director::getInstance()->resume();

    switchToLevelSceneWithScores(levelNumber, {score});
}

std::string GameFlow::getLeaderboardName(int levelNumber)
{
    return "ratnroll_leaderboard_" + std::to_string(levelNumber);
}

Scene *GameFlow::createSceneObject()
{
    Scene *scene = Scene::create();

    std::string backgroundId = "0" + std::to_string(random(1, 3));

    auto backgroundLayer = BackgroundLayer::create("background/bg_item" + backgroundId + ".png",
                                                   "background/bg_plane" + backgroundId + ".png");
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
        return new Level01(iapPurchaseCompleted(), true);
        break;

    default:
        assert(false);
        return new LevelTutorial();
        break;
    }
}

int GameFlow::updateBestScore(int levelNumber, int score)
{
    // local
    int bestScore = PermanentStorage::getInstance().getBestScore(levelNumber);

    // remote
    SonarCocosHelper::GameCenterPlayersScores gameCenterScores =
        SonarCocosHelper::GameCenter::getFriendsBestScores(getLeaderboardName(levelNumber));
    auto i = std::find_if(
        gameCenterScores.begin(), gameCenterScores.end(),
        [](SonarCocosHelper::GameCenterPlayerScore score) { return score.isOwnScore; });
    int remoteBestScore = i != gameCenterScores.end() ? i->score : 0;

    // get max and save locally and remotely
    bestScore = std::max(std::max(remoteBestScore, bestScore), score);

    PermanentStorage::getInstance().setBestScore(levelNumber, bestScore);
    SonarCocosHelper::GameCenter::submitScore(bestScore, getLeaderboardName(levelNumber));

    return bestScore;
}

LevelLayer &GameFlow::getCurrentLevelLayer()
{
    LevelLayer *levelLayer =
        Director::getInstance()->getRunningScene()->getChildByName<LevelLayer *>(LevelLayer::name);
    assert(levelLayer);

    return *levelLayer;
}

std::string GameFlow::getSocialShareMessage(int score)
{
    std::ostringstream os;
    os << "Playing RatNRoll for iOS and just scored " << score << " points!";

    return os.str();
}
