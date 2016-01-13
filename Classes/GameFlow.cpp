#include "GameFlow.h"

#include "SonarFrameworks.h"

#include "PreloadingLayer.h"
#include "InitialLayer.h"
#include "LevelMenuLayer.h"
#include "LevelLayer.h"
#include "LevelSelectionLayer.h"
#include "LevelBlockerLayer.h"
#include "IAPLayer.h"
#include "UnlockGraveyardLayer.h"
#include "UnlockMagicHallLayer.h"
#include "PostLevelLayer.h"
#include "PostTutorialLayer.h"
#include "PauseLayer.h"
#include "SettingsLayer.h"
#include "AboutLayer.h"
#include "GameCompletedLayer.h"
#include "TestLayer.h"
#include "GuaranteedScoreLayer.h"
#include "LevelShadowRatInfoLayer.h"

#include "PermanentStorage.h"

#include "LevelCustomization.h"

#include "AchievementTracker.h"

#include "TransitionScaleChildren.h"

// #define PREDEFINED_SCORE 128932

USING_NS_CC;

static const std::string GACategpryName = "GameFlow";

class SocialSharingHelper
{
  public:
    static void shareOnFacebook()
    {
        SonarCocosHelper::Facebook::Share(
            nullptr, "https://fb.me/561512307335877",
            "Meet not-so-adorable rat running in the rhythm of rock’n’roll!", "Rat'n'roll for iOS",
            imageLink.c_str());
    };

    static void shareOnTwitter(int score)
    {
        SonarCocosHelper::Twitter::Tweet(getSocialShareMessage(score).c_str(), nullptr, "");
    };

    static std::string getSocialShareMessage(int score)
    {
        std::ostringstream os;
        os << "Played RatNRoll for iOS and scored " << score << " points!" << std::endl
           << "https://itunes.apple.com/us/app/ratnroll/id1000398088";

        return os.str();
    }

  private:
    static const std::string imageLink;
};

// http://rafalkucharczyk.github.io/RatNRoll/sharing_image.png
const std::string SocialSharingHelper::imageLink = "http://bit.ly/1Pou3dT";

const std::string GameFlow::iapProductId = "com.nowhere.ratnroll.bonusworlds";

GameFlow *GameFlow::instance = nullptr;

GameFlow::GameFlow()
    : currentBackgroundConfig({"bg_item01"}, "bg_plane01"), currentLevelNumber(noLevelNumber)
{
    loginToGameCenter();
}

GameFlow &GameFlow::getInstance()
{
    if (instance == nullptr) {
        instance = new GameFlow();
    }

    return *instance;
}

Scene *GameFlow::createPreloadingScene()
{
    auto scene = Scene::create();

    auto preloadingScene = PreloadingLayer::create();
    preloadingScene->setPreloadingCompletedCallback(
        std::bind(&GameFlow::switchToInitialScene, this));
    scene->addChild(preloadingScene);

    return scene;
}

void GameFlow::pauseGame()
{
    if (Director::getInstance()->isPaused() || !isGamePlayActive()) {
        return;
    }

    getAchievementTracker().gamePaused();

    getCurrentLevelLayer().pauseLevel();

    Director::getInstance()->pause();
    auto pauseLayer = PauseLayer::create();
    pauseLayer->setGameResumedCallback(std::bind(&GameFlow::resumeGame, this));
    pauseLayer->setGameQuitCallback([this]() {
        this->getCurrentLevelLayer().playBackgroundMusic();
        this->switchToLevelSelectionScene();
    });

    Scene *runningScene = Director::getInstance()->getRunningScene();

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

bool GameFlow::likingCompleted() const { return PermanentStorage::getInstance().getLikingState(); }

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

void GameFlow::replaceScene(cocos2d::Scene *scene)
{
    Director::getInstance()->replaceScene(TransitionScaleChildren::create(0.175, scene));
}

void GameFlow::switchToInitialScene()
{
    SonarCocosHelper::GoogleAnalytics::setScreenName("Initial");

    auto scene = createSceneObject();

    auto initialLayer = InitialLayer::create();

    initialLayer->setPlayCallback(std::bind(&GameFlow::switchToLevelSelectionScene, this));
    initialLayer->setSettingsCallback(std::bind(&GameFlow::switchToSettingsScene, this));
    initialLayer->setGameCenterCallbacks(
        []() { return SonarCocosHelper::GameCenter::isSignedIn(); },
        [this](bool signedIn) {
            if (signedIn) {
                SonarCocosHelper::GameCenter::showLeaderboard();
            } else {
                this->loginToGameCenter();
            }
        });

    scene->addChild(initialLayer);

    replaceScene(scene);
}

void GameFlow::switchToLevelSelectionScene()
{
    currentLevelNumber = noLevelNumber;
    clearCurrentChallenge();

    Director::getInstance()->resume();

    auto scene = createSceneObject();

    bool tutorialCompleted = ((PermanentStorage::getInstance().getTutorialStage() + 1) ==
                              static_cast<int>(TutorialBalloonLayer::BalloonType::FINAL));

    auto levelSelectionLayer = LevelSelectionLayer::create(
        !PermanentStorage::getInstance().getTutorialEntered(),
        tutorialCompleted && !PermanentStorage::getInstance().getCheeseFactoryEntered());

    levelSelectionLayer->setBackButtonClickedCallback([this]() { switchToInitialScene(); });
    levelSelectionLayer->setLevelSelectedCallback([this](int levelNumber) {
        if (levelNumber == 0) {
            PermanentStorage::getInstance().setTutorialEntered(true);
            PermanentStorage::getInstance().setTutorialStage(-1);
        }
        if (levelNumber == 1) {
            PermanentStorage::getInstance().setCheeseFactoryEntered(true);
        }
        switchToLevelScene(levelNumber);
    });

    scene->addChild(levelSelectionLayer);

    replaceScene(scene);
}

void GameFlow::switchToLevelScene(int levelNumber)
{
    SonarCocosHelper::GameCenterPlayersScores scores;

    if (currentChallenge) {
        scores.push_back(*currentChallenge);
    } else {
        scores =
            SonarCocosHelper::GameCenter::getFriendsBestScores(getLeaderboardName(levelNumber));
    }

    switchToLevelSceneWithScores(levelNumber, scores);
}

void GameFlow::switchToLevelSceneWithScores(int levelNumber,
                                            SonarCocosHelper::GameCenterPlayersScores scores)
{
    srand(1); // random, but always the same...

    auto levelCustomization = getLevelCustomization(levelNumber);
    BackgroundConfig tempBackgroundConfig = currentBackgroundConfig;

    setCurrentBackgroundConfig(levelCustomization->getBackgroundConfig());

    auto scene = createSceneObject();

    BackgroundLayer *backgroundLayer =
        dynamic_cast<BackgroundLayer *>(scene->getChildByTag(backgroundLayerTag));

    auto levelMenuLayer = LevelMenuLayer::create();
    scene->addChild(levelMenuLayer);
    levelMenuLayer->setGamePausedCallback(std::bind(&GameFlow::pauseGame, this));

    auto levelShadowRatInfoLayer = LevelShadowRatInfoLayer::create();
    scene->addChild(levelShadowRatInfoLayer);

    auto levelLayer = LevelLayer::create(levelCustomization, addAchievementTracker(*scene),
#ifdef PREDEFINED_SCORE
                                         PREDEFINED_SCORE
#else
                                         PermanentStorage::getInstance().getScoreThresholdForLevel(
                                             levelNumber)
#endif

                                         );

    levelLayer->setShadowRatActionCallback(
        [levelShadowRatInfoLayer](const std::string &playerName, int score, int state) {
            levelShadowRatInfoLayer->showShadowRatInfo(playerName, score, state);
        });

    for (const SonarCocosHelper::GameCenterPlayerScore &score : scores) {
        if (score.isOwnScore) {
            continue;
        }
        levelLayer->addShadowRat(score.playerName, score.getLowerScoreBound(), score.score);
    }

    currentLevelNumber = levelNumber;
    scene->addChild(levelLayer);

    bool levelBlocked =
        blockLevel(*scene, *levelLayer, levelNumber,
                   scores.size() ? scores.front() : SonarCocosHelper::GameCenterPlayerScore());

    if (levelBlocked) {
        setCurrentBackgroundConfig(tempBackgroundConfig);
    }

    levelLayer->setBackgroundSpeedFunction(
        std::bind(&BackgroundLayer::setSpeed, backgroundLayer, std::placeholders::_1));

    SonarCocosHelper::GoogleAnalytics::setScreenName("Level" + std::to_string(levelNumber));

    // special handling for tutorial
    if (levelNumber == 0) {
        levelLayer->setGameCompletedCallback(
            std::bind(&GameFlow::switchToLevelSelectionScene, this));
        Director::getInstance()->replaceScene(scene);
        levelLayer->setGameFinishedCallback(std::bind(&GameFlow::switchToPostTutorialScene, this));
    } else {
        levelLayer->setGameCompletedCallback([this, levelLayer]() {
            this->overlayGameCompletedScene(levelLayer);
            levelLayer->playBackgroundMusic();
        });
        levelLayer->setGameFinishedCallback([this, levelLayer](int score) {
            std::string action = "GameFinished" + std::to_string(currentLevelNumber);
            SonarCocosHelper::GoogleAnalytics::sendEvent(GACategpryName, action, "score", score);
            SonarCocosHelper::GoogleAnalytics::sendEvent(GACategpryName, action, "speed",
                                                         levelLayer->getRatSpeedLevel());
            this->switchToPostLevelScene(score);
            levelLayer->playBackgroundMusic();
        });
        replaceScene(scene);
    }

    if (!levelBlocked) {
        levelLayer->playBackgroundMusic();
    }
}

bool GameFlow::blockLevel(Scene &scene, LevelLayer &levelLayer, int levelNumber,
                          const SonarCocosHelper::GameCenterPlayerScore &score)
{
    Layer *actionLayer = nullptr;

    auto levelBlockerLayer = LevelBlockerLayer::create(levelLayer);
    levelBlockerLayer->setGotoMenuCallback(std::bind(&GameFlow::switchToLevelSelectionScene, this));
    levelBlockerLayer->setTag(levelBlockerTag);

    if (levelNumber == 2 && !likingCompleted()) {
        auto layer = Layer::create();

        auto unlockGraveyardLayer = UnlockGraveyardLayer::create(getBestScore(1, 0) >= 10000);
        unlockGraveyardLayer->setLikingCompletedCallback([levelBlockerLayer, &levelLayer, this]() {
            levelBlockerLayer->unblock();
            PermanentStorage::getInstance().setLikingState(true);
            levelLayer.playBackgroundMusic();
            setCurrentBackgroundConfig(levelLayer.getBackgroundConfig());
        });
        layer->addChild(unlockGraveyardLayer);
        layer->setCascadeOpacityEnabled(true);

        actionLayer = layer;
    }

    if (levelNumber == 3 && !iapPurchaseCompleted()) {
        auto iapLayer = IAPLayer::create(iapProductId, {0.5, 0.78}, 0.15);
        iapLayer->setPurchaseCompletedCallback([levelBlockerLayer, &levelLayer, this]() {
            levelBlockerLayer->unblock();
            SonarCocosHelper::iAds::hideiAdBanner();
            levelLayer.playBackgroundMusic();
            setCurrentBackgroundConfig(levelLayer.getBackgroundConfig());
        });
        auto unlockMagicHallLayer = UnlockMagicHallLayer::create();
        iapLayer->addChild(unlockMagicHallLayer);

        actionLayer = iapLayer;
    }

    if (actionLayer && !score.isChallenge) {
        levelBlockerLayer->addChild(actionLayer);

        scene.addChild(levelBlockerLayer);

        return true;
    }

    return false;
}

void GameFlow::switchToPostLevelScene(int score)
{
    auto scene = createSceneObject();
    addAchievementTracker(*scene);

    auto postLevelLayer =
        PostLevelLayer::create(score, updateBestScore(currentLevelNumber, score),
                               currentChallenge ? currentChallenge->score : 0,
                               currentChallenge ? currentChallenge->playerName : "");

    int _currentLevelNumber = this->currentLevelNumber;
    postLevelLayer->setRestartLevelCallback([this, _currentLevelNumber]() {
        this->switchToLevelScene(_currentLevelNumber);
        SonarCocosHelper::GoogleAnalytics::sendEvent(GACategpryName, "GameRestarted", "levelNumber",
                                                     _currentLevelNumber);
    });
    postLevelLayer->setGotoMainMenuCallback(
        std::bind(&GameFlow::switchToLevelSelectionScene, this));
    postLevelLayer->setShareOnFacebookCallback([this, score]() {
        SocialSharingHelper::shareOnFacebook();
        getAchievementTracker().scoreShared();
    });
    postLevelLayer->setShareOnTwitterCallback([this, score]() {
        SocialSharingHelper::shareOnTwitter(score);
        getAchievementTracker().scoreShared();
    });
    scene->addChild(postLevelLayer);

    if (score < LevelCustomization::gameCompletedScore) {
        overlayGameScoreThresholdScene(score, scene);
    }

    currentLevelNumber = noLevelNumber;

    SonarCocosHelper::GoogleAnalytics::setScreenName("PostLevel");
    replaceScene(scene);
}

void GameFlow::switchToPostTutorialScene()
{
    auto scene = createSceneObject();
    addAchievementTracker(*scene);

    auto postLevelLayer = PostTutorialLayer::create();
    postLevelLayer->setRestartLevelCallback(
        std::bind(&GameFlow::switchToLevelScene, this, currentLevelNumber));
    postLevelLayer->setGotoMainMenuCallback(
        std::bind(&GameFlow::switchToLevelSelectionScene, this));
    scene->addChild(postLevelLayer);

    currentLevelNumber = noLevelNumber;

    SonarCocosHelper::GoogleAnalytics::setScreenName("PostTutorial");
    replaceScene(scene);
}

void GameFlow::switchToSettingsScene()
{
    auto scene = createSceneObject();

    auto settingsLayer = SettingsLayer::create(getSoundSettings());
    auto iapLayer = IAPLayer::create(iapProductId, {0.35 - 0.02, 0.45 + 0.02}, 0.15, true);

    settingsLayer->setGotoMainMenuCallback([this]() { switchToInitialScene(); });
    settingsLayer->setGotoAboutPageCallback([this]() { switchToAboutScene(); });

    iapLayer->setPurchaseCompletedCallback([]() { SonarCocosHelper::iAds::hideiAdBanner(); });
    settingsLayer->setSoundSettingsChangedCallback([this](const SoundSettings &soundSettings) {
        SoundHelper::getInstance().configure(soundSettings);
        setSoundSettings(soundSettings);
    });
    scene->addChild(iapLayer);
    scene->addChild(settingsLayer);

    SonarCocosHelper::GoogleAnalytics::setScreenName("Settings");
    replaceScene(scene);
}

void GameFlow::switchToAboutScene()
{
    auto scene = createSceneObject();
    auto &achivementTracker = addAchievementTracker(*scene);

    auto aboutLayer = AboutLayer::create();

    aboutLayer->setGotoMainMenuCallback([this]() {
        getAchievementTracker().creditsExited();
        switchToSettingsScene();
    });

    aboutLayer->setResetGameStateCallback([]() {
        SonarCocosHelper::GameCenter::resetPlayerAchievements();
        PermanentStorage::getInstance().setAchievementTrackerState(AchievementTracker::State());
        PermanentStorage::getInstance().setUnlockedAchievements(PermanentStorage::CustomDataMap());

        PermanentStorage::getInstance().setPurchaseState(GameFlow::iapProductId, false);
        PermanentStorage::getInstance().setLikingState(false);
        PermanentStorage::getInstance().setTutorialEntered(false);
        PermanentStorage::getInstance().setCheeseFactoryEntered(false);
        PermanentStorage::getInstance().setTutorialStage(-1);
        PermanentStorage::getInstance().setScoreThresholdForLevel(1, 0);
        PermanentStorage::getInstance().setScoreThresholdForLevel(2, 0);
        PermanentStorage::getInstance().setScoreThresholdForLevel(3, 0);
    });

    aboutLayer->setGotoTestLayerCallback([this]() { switchToTestScene(); });

    scene->addChild(aboutLayer);

    achivementTracker.creditsEntered();
    SonarCocosHelper::GoogleAnalytics::setScreenName("About");
    replaceScene(scene);
}

void GameFlow::switchToTestScene()
{
#ifdef COCOS2D_DEBUG
    auto scene = Scene::create();

    auto layer = TestLayer::create();

    layer->setGotoMainMenuCallback([this]() { switchToInitialScene(); });

    scene->addChild(layer);

    replaceScene(scene);
#endif
}

void GameFlow::overlayGameCompletedScene(LevelLayer *levelLayer)
{
    getAchievementTracker().levelCompleted(currentLevelNumber);

    auto gameCompletionLayer = GameCompletedLayer::create(currentLevelNumber);
    levelLayer->pauseLevel();
    levelLayer->setTag(levelCompletedTag);
    Director::getInstance()->getRunningScene()->addChild(gameCompletionLayer);
    gameCompletionLayer->setCompletionConifrmedCallback([this, levelLayer]() {
        this->switchToPostLevelScene(LevelCustomization::gameCompletedScore);
    });

    SonarCocosHelper::GoogleAnalytics::sendEvent(GACategpryName, "GameCompleted", "levelNumber",
                                                 currentLevelNumber);
}

void GameFlow::overlayGameScoreThresholdScene(int gameScore, cocos2d::Scene *parentScene)
{
    auto gameScoreThreshold = getCurrentLevelLayer().getFixedScoreThresholdForGameScore(gameScore);

    if (gameScoreThreshold.first >
        PermanentStorage::getInstance().getScoreThresholdForLevel(currentLevelNumber)) {
        PermanentStorage::getInstance().setScoreThresholdForLevel(currentLevelNumber,
                                                                  gameScoreThreshold.first);

        auto layer = GuaranteedScoreLayer::create(gameScore, gameScoreThreshold.first,
                                                  gameScoreThreshold.second);

        parentScene->addChild(layer, 1);

        SonarCocosHelper::GoogleAnalytics::sendEvent(
            GACategpryName, "ThresholdCompleted" + std::to_string(currentLevelNumber), "score",
            gameScoreThreshold.first);
    }
}

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

    currentChallenge.reset(new SonarCocosHelper::GameCenterPlayerScore(score));

    switchToLevelScene(levelNumber);
}

std::string GameFlow::getLeaderboardName(int levelNumber)
{
    if (levelNumber == 0) {
        return ""; // no leaderboard for tutorial
    }

    return "ratnroll_leaderboard_" + std::to_string(levelNumber);
}

void GameFlow::clearCurrentChallenge()
{
    currentChallenge.reset();
    SonarCocosHelper::GameCenter::clearCurrentChallenge();
}

Scene *GameFlow::createSceneObject()
{
    Scene *scene = Scene::create();

    auto backgroundLayer = BackgroundLayer::create(currentBackgroundConfig);
    scene->addChild(backgroundLayer, 0, backgroundLayerTag);

    return scene;
}

void GameFlow::setCurrentBackgroundConfig(const BackgroundConfig &backgroundConfig)
{
    currentBackgroundConfig = backgroundConfig;
}

LevelCustomization *GameFlow::getLevelCustomization(int levelNumber) const
{
    switch (levelNumber) {
    case 0:
        return new LevelTutorial();
        break;

    case 1:
        return new Level01(iapPurchaseCompleted(), likingCompleted());
        break;

    case 2:
        return new Level02(iapPurchaseCompleted(), true);
        break;

    case 3:
        return new Level03(true, likingCompleted());
        break;

    default:
        assert(false);
        return new LevelTutorial();
        break;
    }
}

int GameFlow::getBestScore(int levelNumber, int currentScore)
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

    return std::max(std::max(remoteBestScore, bestScore), currentScore);
}

int GameFlow::updateBestScore(int levelNumber, int currentScore)
{
    int bestScore = getBestScore(levelNumber, currentScore);

    PermanentStorage::getInstance().setBestScore(levelNumber, bestScore);
#ifndef PREDEFINED_SCORE
    SonarCocosHelper::GameCenter::submitScore(bestScore, getLeaderboardName(levelNumber));
#endif
    return bestScore;
}

LevelLayer &GameFlow::getCurrentLevelLayer()
{
    LevelLayer *levelLayer =
        Director::getInstance()->getRunningScene()->getChildByName<LevelLayer *>(LevelLayer::name);
    assert(levelLayer);

    return *levelLayer;
}

bool GameFlow::isGamePlayActive() const
{
    Scene *scene = Director::getInstance()->getRunningScene();

    return (currentLevelNumber != noLevelNumber) && scene != nullptr &&
           scene->getChildByTag(levelBlockerTag) == nullptr &&
           scene->getChildByTag(LevelLayer::proxyOverlayLayerTag) == nullptr &&
           scene->getChildByTag(levelCompletedTag) == nullptr;
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
