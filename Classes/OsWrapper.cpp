#include "cocos2d.h"
#include "OsWrapper.h"
#include "SonarFrameworks.h"

using namespace SonarCocosHelper;
USING_NS_CC;

std::vector<ButtonConfig::Action> menuItemsActions = {
    [](Node *node) {},

    [](Node *node) {
        FiniteTimeAction *moveLeft =
            MoveBy::create(0.04, Vec2(-0.05 * node->getContentSize().width, 0));
        FiniteTimeAction *moveRight =
            MoveBy::create(0.04, Vec2(0.05 * node->getContentSize().width, 0));
        node->runAction(Sequence::create(
            Repeat::create(Sequence::create(moveLeft, moveLeft->reverse(), moveRight,
                                            moveRight->reverse(), nullptr),
                           3),
            nullptr));
    }};

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

void OsWrapper::signIn(std::function<void()> signedInCallback)
{
    GameCenter::signIn(signedInCallback);
}

bool OsWrapper::isSignedIn() { return GameCenter::isSignedIn(); }

void OsWrapper::showLeaderboards(int itemId) { GameCenter::showLeaderboard(); }

void OsWrapper::registerChallengeCallback(std::function<void(GameCenterPlayerScore)> callback)
{
    GameCenter::registerChallengeCallback(callback);
}

GameCenterPlayersScores OsWrapper::getFriendsBestScores(const std::string &leaderboardID)
{
    return GameCenter::getFriendsBestScores(leaderboardID);
}

void OsWrapper::showAdBannerWithCallbacks(int position, std::function<void(bool)> adShownCallback,
                                          std::function<void()> adHiddenCallback)
{
    iAds::showiAdBannerWithCallbacks(SonarCocosHelper::eBottom, adShownCallback, adHiddenCallback);
}

std::vector<ButtonConfig> OsWrapper::getInitMenuLayout()
{
    return std::vector<ButtonConfig>({
        {{0.5, 0.625}, 0.35, "play"},
        {{0.35, 0.25}, 0.125, "settings"},
        {{0.65, 0.25}, 0.125, "gamecenter", menuItemsActions},
    });
}

#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

void OsWrapper::signIn(std::function<void()> signedInCallback) { GooglePlayServices::signIn(); }

bool OsWrapper::isSignedIn()
{
    bool status = GooglePlayServices::isSignedIn();

    return status;
}

void OsWrapper::showLeaderboards(int itemId)
{
    if (itemId == 3) {
        GooglePlayServices::showAchievements();
    } else {
        GooglePlayServices::showLeaderboards();
    }
}

void OsWrapper::registerChallengeCallback(std::function<void(GameCenterPlayerScore)> callback)
{
    CCLOG("%s not implemented for Android", __func__);
}

GameCenterPlayersScores OsWrapper::getFriendsBestScores(const std::string &leaderboardID)
{
    CCLOG("%s not implemented for Android", __func__);
    return GameCenterPlayersScores();
}

void OsWrapper::showAdBannerWithCallbacks(int position, std::function<void(bool)> adShownCallback,
                                          std::function<void()> adHiddenCallback)
{
    SonarCocosHelper::AdMob::showBannerAd(SonarCocosHelper::eBottom);
}

std::vector<ButtonConfig> OsWrapper::getInitMenuLayout()
{
    return std::vector<ButtonConfig>({
        {{0.5, 0.625}, 0.35, "play"},
        {{0.25, 0.25}, 0.125, "settings"},
        {{0.5, 0.25}, 0.125, "leaderboards", menuItemsActions},
        {{0.75, 0.25}, 0.125, "achievements", menuItemsActions},
    });
}

#endif /* (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) */
