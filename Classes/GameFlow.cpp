#include "GameFlow.h"

#include "SonarFrameworks.h"

#include "BackgroundLayer.h"
#include "InitialLayer.h"
#include "LevelMenuLayer.h"
#include "LevelLayer.h"
#include "LevelSelectionLayer.h"
#include "PostLevelLayer.h"
#include "PauseLayer.h"
#include "SettingsLayer.h"
#include "AboutLayer.h"
#include "TestLayer.h"

#include "PermanentStorage.h"

#include "LevelCustomization.h"
#include "InAppPurchaseHelper.h"

#include "AchievementTracker.h"

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

    getAchievementTracker().gamePaused();

    getCurrentLevelLayer().pauseLevel();

    Director::getInstance()->pause();
    auto pauseLayer = PauseLayer::create();
    pauseLayer->setGameResumedCallback(std::bind(&GameFlow::resumeGame, this));
    pauseLayer->setGameQuitCallback(std::bind(&GameFlow::switchToLevelSelectionScene, this));

    Scene *runningScene = Director::getInstance()->getRunningScene();

    if (runningScene->getChildByTag(LevelLayer::proxyOverlayLayer)) {
        runningScene->removeChildByTag(LevelLayer::proxyOverlayLayer);
    }

    runningScene->addChild(pauseLayer, 0, pauseLayerTag);
}

void GameFlow::resumeGame()
{
    if (!Director::getInstance()->isPaused()) {
        return;
    }

    getAchievementTracker().gameResumed();

    Director::getInstance()->resume();

    Director::getInstance()->getRunningScene()->removeChildByTag(pauseLayerTag);

    getCurrentLevelLayer().resumeLevel();
}

bool GameFlow::iapPurchaseCompleted() const
{
    return InAppPurchaseHelper::isPurchased(GameFlow::iapProductId);
}

SoundSettings GameFlow::getSoundSettings() const
{
    return PermanentStorage::getInstance().getSoundSettings();
}

void GameFlow::setSoundSettings(const SoundSettings &settings)
{
    PermanentStorage::getInstance().setSoundSettings(settings);
}

void GameFlow::switchToInitialScene()
{
    SonarCocosHelper::GoogleAnalytics::setScreenName("Initial");
    Director::getInstance()->replaceScene(createInitialScene());
}

void GameFlow::handleInitialSceneMenu(int itemIndex)
{
    if (itemIndex == 0) {
        switchToLevelSelectionScene();
    } else if (itemIndex == 1) {
        switchToSettingsScene();
    } else if (itemIndex == 2) {
        SonarCocosHelper::GameCenter::showLeaderboard();
    } else if (itemIndex == 3) {
        switchToAboutScene();
    }
}

void GameFlow::switchToLevelSelectionScene()
{
    currentLevelNumber = noLevelNumber;

    Director::getInstance()->resume();

    auto scene = createSceneObject();

    auto levelSelectionLayer = LevelSelectionLayer::create();

    levelSelectionLayer->setBackButtonClickedCallback([this]() { switchToInitialScene(); });
    levelSelectionLayer->setLevelSelectedCallback(
        [this](int levelNumber) { switchToLevelScene(levelNumber); });

    scene->addChild(levelSelectionLayer);

    Director::getInstance()->replaceScene(scene);
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

    auto levelCustomization = getLevelCustomization(levelNumber);

    auto scene = createSceneObject(levelCustomization->getBgPlaneName(),
                                   levelCustomization->getBgItemNames());
    BackgroundLayer *backgroundLayer =
        dynamic_cast<BackgroundLayer *>(scene->getChildByTag(backgroundLayerTag));

    auto levelMenuLayer = LevelMenuLayer::create();
    scene->addChild(levelMenuLayer);
    levelMenuLayer->setGamePausedCallback(std::bind(&GameFlow::pauseGame, this));

    auto levelLayer = LevelLayer::create(levelCustomization, addAchievementTracker(*scene));

    for (const SonarCocosHelper::GameCenterPlayerScore &score : scores) {
        levelLayer->addShadowRat(score.playerName, score.getLowerScoreBound(), score.score);
    }

    currentLevelNumber = levelNumber;
    scene->addChild(levelLayer);

    levelLayer->setGameFinishedCallback(
        std::bind(&GameFlow::switchToPostLevelScene, this, std::placeholders::_1));
    levelLayer->setBackgroundSpeedFunction(
        std::bind(&BackgroundLayer::setSpeed, backgroundLayer, std::placeholders::_1));

    SonarCocosHelper::GoogleAnalytics::setScreenName("Level" + std::to_string(levelNumber));
    Director::getInstance()->replaceScene(scene);
}

void GameFlow::switchToPostLevelScene(int score)
{
    auto scene = createSceneObject();
    addAchievementTracker(*scene);

    auto postLevelLayer = PostLevelLayer::create();
    postLevelLayer->displayCurrentScore(score);
    postLevelLayer->displayBestScore(updateBestScore(currentLevelNumber, score));
    postLevelLayer->setRestartLevelCallback(
        std::bind(&GameFlow::switchToLevelScene, this, currentLevelNumber));
    postLevelLayer->setGotoMainMenuCallback(
        std::bind(&GameFlow::switchToLevelSelectionScene, this));
    postLevelLayer->setShareOnFacebookCallback([this, score]() {
        SonarCocosHelper::Facebook::Share(nullptr, nullptr, getSocialShareMessage(score).c_str(),
                                          nullptr, "");
        getAchievementTracker().scoreShared();
    });
    postLevelLayer->setShareOnTwitterCallback([this, score]() {
        SonarCocosHelper::Twitter::Tweet(getSocialShareMessage(score).c_str(), nullptr, "");
        getAchievementTracker().scoreShared();
    });
    scene->addChild(postLevelLayer);

    currentLevelNumber = noLevelNumber;

    SonarCocosHelper::GoogleAnalytics::setScreenName("PostLevel");
    Director::getInstance()->replaceScene(scene);
}

void GameFlow::switchToSettingsScene()
{
    auto scene = createSceneObject();

    auto settingsLayer = SettingsLayer::createNoInit(getSoundSettings());
    iapHelper.reset(new InAppPurchaseHelper(iapProductId, *settingsLayer));
    settingsLayer->init([this]() { iapHelper->init(); });

    settingsLayer->setGotoMainMenuCallback([this]() {
        iapHelper.reset();
        switchToInitialScene();
    });
    settingsLayer->setPurchaseRequestedCallback([this]() { iapHelper->purchaseProduct(); });
    settingsLayer->setPurchaseCompletedCallback([]() { SonarCocosHelper::iAds::hideiAdBanner(); });
    settingsLayer->setSoundSettingsChangedCallback([this](const SoundSettings &soundSettings) {
        SoundHelper::getInstance().init(soundSettings);
        setSoundSettings(soundSettings);
    });
    scene->addChild(settingsLayer);

    SonarCocosHelper::GoogleAnalytics::setScreenName("Settings");
    Director::getInstance()->replaceScene(scene);
}

void GameFlow::switchToAboutScene()
{
    auto scene = createSceneObject();
    auto &achivementTracker = addAchievementTracker(*scene);

    auto aboutLayer = AboutLayer::create();

    aboutLayer->setGotoMainMenuCallback([this]() {
        getAchievementTracker().creditsExited();
        switchToInitialScene();
    });

    aboutLayer->setResetGameStateCallback([]() {
        SonarCocosHelper::GameCenter::resetPlayerAchievements();
        PermanentStorage::getInstance().setAchievementTrackerState(AchievementTracker::State());
        PermanentStorage::getInstance().setUnlockedAchievements(PermanentStorage::CustomDataMap());

        PermanentStorage::getInstance().setPurchaseState(GameFlow::iapProductId, false);
    });

    scene->addChild(aboutLayer);

    achivementTracker.creditsEntered();
    SonarCocosHelper::GoogleAnalytics::setScreenName("About");
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

void GameFlow::loginToGameCenter()
{
    SonarCocosHelper::GameCenter::registerChallengeCallback(
        std::bind(&GameFlow::startChallenge, this, std::placeholders::_1));

    SonarCocosHelper::GameCenter::signIn([this]() {
        for (int levelNumber = 1; levelNumber <= 3; levelNumber++) {
            SonarCocosHelper::GameCenter::getFriendsBestScores(getLeaderboardName(levelNumber));
        }
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
    if (levelNumber == 0) {
        return ""; // no leaderboard for tutorial
    }

    return "ratnroll_leaderboard_" + std::to_string(levelNumber);
}

Scene *GameFlow::createSceneObject(const std::string &bgPlaneName,
                                   const std::list<std::string> &bgItemNames)
{
    Scene *scene = Scene::create();

    std::vector<std::string> itemFileNames(bgItemNames.size());

    std::transform(bgItemNames.begin(), bgItemNames.end(), itemFileNames.begin(),
                   [](const std::string &i) { return "background/" + i + ".png"; });

    auto backgroundLayer =
        BackgroundLayer::create(itemFileNames, "background/" + bgPlaneName + ".png");
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

    case 2:
        return new Level02(iapPurchaseCompleted(), true);
        break;

    case 3:
        return new Level03(iapPurchaseCompleted(), true);
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

AchievementTracker &GameFlow::addAchievementTracker(cocos2d::Node &parent) const
{
    Node *node = parent.getChildByTag(achievementTrackerTag);

    if (node == nullptr) {
        node = AchievementTracker::create();
        parent.addChild(node, 0, achievementTrackerTag);
    }

    return static_cast<AchievementTracker &>(*node);
}

AchievementTracker &GameFlow::getAchievementTracker() const
{
    Node *parent = Director::getInstance()->getRunningScene();

    assert(parent);

    Node *node = parent->getChildByTag(achievementTrackerTag);

    assert(node);

    return static_cast<AchievementTracker &>(*node);
}

std::string GameFlow::getSocialShareMessage(int score)
{
    std::ostringstream os;
    os << "Playing RatNRoll for iOS and just scored " << score << " points!";

    return os.str();
}
