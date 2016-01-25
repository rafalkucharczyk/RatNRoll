/*
 Copyright (C) 2015 Sonar Systems - All Rights Reserved
 You may use, distribute and modify this code under the
 terms of the MIT license

 Any external frameworks used have their own licenses and
 should be followed as such.
 */
//
//  SonarFrameworks.cpp
//  Sonar Cocos Helper
//
//  Created by Sonar Systems on 03/03/2015.
//

#include "SonarFrameworks.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include <android/log.h>
#define CLASS_NAME "sonar/systems/framework/SonarFrameworkFunctions"
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "IOSCPPHelper.h"
#endif

using namespace cocos2d;
using namespace SonarCocosHelper;

void IOS::Setup()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    IOSCPPHelper::Setup();
#endif
}

void IOS::Share(cocos2d::__String shareString, cocos2d::__String imagePath)
{
#if SCH_IS_SOCIAL_ENABLED == true
    IOSCPPHelper::shareWithString(shareString, imagePath);
#endif
}

bool GooglePlayServices::isSignedIn()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonBoolCall("isSignedIn", CLASS_NAME);
#endif

    return false;
}

void GooglePlayServices::signIn()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniHelpers::jniCommonVoidCall("gameServicesSignIn", CLASS_NAME);
#endif
}

void GooglePlayServices::signOut()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniHelpers::jniCommonVoidCall("gameServicesSignOut", CLASS_NAME);
#endif
}

void GooglePlayServices::submitScore(const char *leaderboardID, long score)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniHelpers::jniCommonVoidCall("submitScore", CLASS_NAME, leaderboardID, score);
#endif
}

void GooglePlayServices::unlockAchievement(const char *achievementID)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniHelpers::jniCommonVoidCall("unlockAchievement", CLASS_NAME, achievementID);
#endif
}

void GooglePlayServices::incrementAchievement(const char *achievementID, int numSteps)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniHelpers::jniCommonVoidCall("incrementAchievement", CLASS_NAME, achievementID, numSteps);
#endif
}

void GooglePlayServices::showAchievements()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniHelpers::jniCommonVoidCall("showAchievements", CLASS_NAME);
#endif
}

void GooglePlayServices::showLeaderboards()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniHelpers::jniCommonVoidCall("showLeaderboards", CLASS_NAME);
#endif
}

void GooglePlayServices::showLeaderboard(const char *leaderboardID)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniHelpers::jniCommonVoidCall("showLeaderboard", CLASS_NAME, leaderboardID);
#endif
}

void GameCenter::signIn(std::function<void()> signedInCallback)
{
#if SCH_IS_GAME_CENTER_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::gameCenterLogin(signedInCallback);
#endif
}

bool GameCenter::isSignedIn()
{
#if SCH_IS_GAME_CENTER_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    return IOSCPPHelper::gameCenterIsLoggedIn();
#endif
    return false;
}

void GameCenter::showLeaderboard()
{
#if SCH_IS_GAME_CENTER_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::gameCenterShowLeaderboard();
#endif
}

void GameCenter::showAchievements()
{
#if SCH_IS_GAME_CENTER_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::gameCenterShowAchievements();
#endif
}

void GameCenter::hideUI()
{
#if SCH_IS_GAME_CENTER_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::gameCenterHideUi();
#endif
}

void GameCenter::submitScore(int scoreNumber, cocos2d::__String leaderboardID)
{
#if SCH_IS_GAME_CENTER_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    if (leaderboardID.length() != 0) {
        IOSCPPHelper::gameCenterSubmitScore(scoreNumber, leaderboardID);
    }
    IOSCPPHelper::gameCenterSubmitScoreForChallenge(scoreNumber);
#endif
}

void GameCenter::unlockAchievement(cocos2d::__String achievementID)
{
#if SCH_IS_GAME_CENTER_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::gameCenterUnlockAchievement(achievementID, 100.0f);
#endif
}

void GameCenter::unlockAchievement(cocos2d::__String achievementID, float percent)
{
#if SCH_IS_GAME_CENTER_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::gameCenterUnlockAchievement(achievementID, percent);
#endif
}

void GameCenter::resetPlayerAchievements()
{
#if SCH_IS_GAME_CENTER_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::gameCenterResetPlayerAchievements();
#endif
}

void GameCenter::registerChallengeCallback(std::function<void(GameCenterPlayerScore)> callback)
{
#if SCH_IS_GAME_CENTER_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::gameCenterRegisterChallengeCallback(callback);
#endif
}

void GameCenter::clearCurrentChallenge()
{
#if SCH_IS_GAME_CENTER_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::gameCenterClearCurrentChallenge();
#endif
}

GameCenterPlayersScores GameCenter::getFriendsBestScores(const std::string &leaderboardID)
{
#if SCH_IS_GAME_CENTER_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    return IOSCPPHelper::gameCenterGetFriendsBestScores(leaderboardID);
#else
    return {};
#endif
}

cocos2d::Sprite *GameCenter::getImageForPlayer(const std::string &playerName)
{
#if SCH_IS_GAME_CENTER_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    return IOSCPPHelper::getImageForPlayer(playerName);
#else
    return nullptr;
#endif
}

/*void Facebook::SignIn()
{
    #if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            return JniHelpers::jniCommonVoidCall(
                "FacebookSignIn",
                CLASS_NAME);
    #elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        //TODO
    #endif
}*/

void Facebook::Share(const char *name, const char *link, const char *description,
                     const char *caption, const char *imagePath)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("FacebookShare", CLASS_NAME, name, link, description,
                                         caption, imagePath);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_SOCIAL_ENABLED == true
    IOSCPPHelper::shareLinkViaFacebook(link, imagePath, caption, description);
#endif
#endif
}

void Twitter::Tweet(const char *tweet, const char *title, const char *imagePath)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniHelpers::jniCommonVoidCall("TwitterTweet", CLASS_NAME, tweet, title);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_SOCIAL_ENABLED == true
    IOSCPPHelper::shareViaTwitter(tweet, imagePath);
#endif
#endif
}

void Mopub::showBannerAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("ShowBannerAdMP", CLASS_NAME);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_MOPUB_ENABLED == true
    IOSCPPHelper::showMopubBanner();
#endif
#endif
}

void Mopub::hideBannerAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("HideBannerAdMP", CLASS_NAME);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_MOPUB_ENABLED == true
    IOSCPPHelper::hideMopubBanner();
#endif
#endif
}

void Mopub::showFullscreenAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("ShowFullscreenAdMP", CLASS_NAME);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_MOPUB_ENABLED == true
    IOSCPPHelper::showMoPubFullscreenAd();
#endif
#endif
}

void AdMob::showBannerAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("ShowBannerAd", CLASS_NAME, AdBannerPosition::eTop);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_AD_MOB_ENABLED == true
    IOSCPPHelper::showAdMobBanner(AdBannerPosition::eTop);
#endif
#endif
}

void AdMob::showBannerAd(int position)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("ShowBannerAd", CLASS_NAME, position);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_AD_MOB_ENABLED == true
    IOSCPPHelper::showAdMobBanner(position);
#endif
#endif
}

void AdMob::hideBannerAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("HideBannerAd", CLASS_NAME);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_AD_MOB_ENABLED == true
    IOSCPPHelper::hideAdMobBanner(eBoth);
#endif
#endif
}

void AdMob::hideBannerAd(int position)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("HideBannerAd", CLASS_NAME, position);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_AD_MOB_ENABLED == true
    IOSCPPHelper::hideAdMobBanner(position);
#endif
#endif
}

void AdMob::showFullscreenAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("ShowFullscreenAdAM", CLASS_NAME);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_AD_MOB_ENABLED == true
    IOSCPPHelper::showAdMobFullscreenAd();
#endif
#endif
}

void AdMob::preLoadFullscreenAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("PreLoadFullscreenAdAM", CLASS_NAME);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_AD_MOB_ENABLED == true
// todo
#endif
#endif
}

void AdMob::showPreLoadedFullscreenAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("ShowPreLoadedFullscreenAdAM", CLASS_NAME);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_AD_MOB_ENABLED == true
// todo
#endif
#endif
}

void RevMob::showFullscreenAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("ShowFullscreenAd", CLASS_NAME);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_REVMOB_ENABLED == true
    IOSCPPHelper::showRevMobFullScreenAd();
#endif
#endif
}

void RevMob::showPopupAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("ShowPopUpAd", CLASS_NAME);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_REVMOB_ENABLED == true
    IOSCPPHelper::showRevMobPopupAd();
#endif
#endif
}

void RevMob::showBannerAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_REVMOB_ENABLED == true
    IOSCPPHelper::showRevMobBanner();
#endif
#endif
}

void RevMob::hideBannerAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_REVMOB_ENABLED == true
    IOSCPPHelper::hideRevMobBanner();
#endif
#endif
}

void Chartboost::showFullscreenAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("ShowFullscreenAdCB", CLASS_NAME);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_CHARTBOOST_ENABLED == true
    IOSCPPHelper::showChartboostFullScreenAd();
#endif
#endif
}

void Chartboost::preLoadFullscreenAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("PreLoadFullscreenAdCB", CLASS_NAME);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_AD_MOB_ENABLED == true
// todo
#endif
#endif
}

void Chartboost::preLoadVideoAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("PreLoadVideoAdCB", CLASS_NAME);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_AD_MOB_ENABLED == true
// todo
#endif
#endif
}

void Chartboost::showVideoAd()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("ShowVideoAdCB", CLASS_NAME);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_CHARTBOOST_ENABLED == true
    IOSCPPHelper::showChartboostVideoAd();
#endif
#endif
}

void Chartboost::showMoreApps()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_CHARTBOOST_ENABLED == true
    IOSCPPHelper::showChartboostMoreApps();
#endif
#endif
}

void iAds::showiAdBanner()
{
#if SCH_IS_iADS_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::showiAdBanner(AdBannerPosition::eTop);
#endif
}

void iAds::showiAdBanner(int position)
{
#if SCH_IS_iADS_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::showiAdBanner(position);
#endif
}

void iAds::showiAdBannerWithCallbacks(int position, std::function<void(bool)> adShownCallback,
                                      std::function<void()> adHiddenCallback)
{
#if SCH_IS_iADS_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::showiAdBannerWithCallbacks(position, adShownCallback, adHiddenCallback);
#endif
}

void iAds::hideiAdBanner()
{
#if SCH_IS_iADS_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::hideiAdBanner();
#endif
}

void GoogleAnalytics::setScreenName(cocos2d::__String screenName)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("SetGAScreenName", CLASS_NAME, screenName.getCString());

#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_GOOGLE_ANALYTICS_ENABLED == true
    IOSCPPHelper::setGAScreenName(screenName);
#endif
#endif
}

void GoogleAnalytics::setDispatchInterval(int dispatchInterval)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("SetGADispatchInterval", CLASS_NAME, dispatchInterval);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_GOOGLE_ANALYTICS_ENABLED == true
    IOSCPPHelper::setGADispatchInterval(dispatchInterval);
#endif
#endif
}

void GoogleAnalytics::sendEvent(cocos2d::__String category, cocos2d::__String action,
                                cocos2d::__String label, long value)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("SendGAEvent", CLASS_NAME, category.getCString(),
                                         action.getCString(), label.getCString(), value);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_GOOGLE_ANALYTICS_ENABLED == true
    IOSCPPHelper::sendGAEvent(category, action, label, value);
#endif
#endif
}

void AdColony::showVideoAC(bool withPreOp, bool withPostOp)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("ShowRewardedVideoAdAC", CLASS_NAME);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_ADCOLONY_ENABLED == true
    IOSCPPHelper::showVideoAC(withPreOp, withPostOp);
#endif
#endif
}

void Vungle::ShowVideoVungle(bool isIncentivised)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    return JniHelpers::jniCommonVoidCall("ShowRewardedVideoAdV", CLASS_NAME, isIncentivised);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#if SCH_IS_VUNGLE_ENABLED == true
    IOSCPPHelper::showVideoVungle(isIncentivised);
#endif
#endif
}

// NOT WORKING ATM
/*
void Everyplay::setup( )
{
#if SCH_IS_EVERYPLAY_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::setupEveryplay( );
#endif
}

void Everyplay::showEveryplay( )
{
#if SCH_IS_EVERYPLAY_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::showEveryplay( );
#endif
}

void Everyplay::record( )
{
#if SCH_IS_EVERYPLAY_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::recordEveryplayVideo( );
#endif
}

void Everyplay::playLastVideoRecording( )
{
#if SCH_IS_EVERYPLAY_ENABLED == true && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    IOSCPPHelper::playLastEveryplayVideoRecording( );
#endif
}
*/
