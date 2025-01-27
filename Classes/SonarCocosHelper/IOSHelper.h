/*
 Copyright (C) 2015 Sonar Systems - All Rights Reserved
 You may use, distribute and modify this code under the
 terms of the MIT license

 Any external frameworks used have their own licenses and
 should be followed as such.
 */
//
//  IOSHelper.h
//  Sonar Cocos Helper
//
//  Created by Sonar Systems on 03/03/2015.
//
//

#import "SCHDefinitions.h"

#import "SCHSettings.h"

#import <Foundation/Foundation.h>
#import "AppController.h"

#if SCH_IS_iADS_ENABLED == true
#import <iAd/iAd.h>
#endif

#if SCH_IS_CHARTBOOST_ENABLED == true
#import <Chartboost/Chartboost.h>
#endif

#if SCH_IS_REVMOB_ENABLED == true
#import <RevMobAds/RevMobAds.h>
#endif

#if SCH_IS_AD_MOB_ENABLED == true
#import <GoogleMobileAds/GoogleMobileAds.h>
#endif

#if SCH_IS_EVERYPLAY_ENABLED == true
#import <Everyplay/Everyplay.h>
#endif

#if SCH_IS_MOPUB_ENABLED == true
#import <MPAdView.h>
#import <MPInterstitialAdController.h>
#endif

#if SCH_IS_GOOGLE_ANALYTICS_ENABLED == true
#import <GAI.h>
#import <GAIDictionaryBuilder.h>
#import <GAIEcommerceProduct.h>
#import <GAIEcommerceProductAction.h>
#import <GAIEcommercePromotion.h>
#import <GAIFields.h>
#import <GAILogger.h>
#import <GAITrackedViewController.h>
#import <GAITracker.h>
#import <GAIEcommerceFields.h>
#import <GAITrackedViewController.h>
#endif

#if SCH_IS_ADCOLONY_ENABLED == true
#import <AdColony/AdColony.h>
#endif

#if SCH_IS_VUNGLE_ENABLED == true
#import <VungleSDK/VungleSDK.h>
#endif

#if SCH_IS_GAME_CENTER_ENABLED == true
#import <GameKit/GameKit.h>
#endif

@protocol SCHEmptyProtocol
@end

#if SCH_IS_iADS_ENABLED == true
typedef void (*ADBannerShownCallback)(bool stillActive);
typedef void (*ADBannerHiddenCallback)();
#endif

#if SCH_IS_GAME_CENTER_ENABLED == true
typedef void (*GameCenterChallengeCallback)(GKScoreChallenge *);

#endif

@interface IOSHelper : NSObject <SCHEmptyProtocol
#if SCH_IS_iADS_ENABLED == true
                                 ,
                                 ADBannerViewDelegate
#endif

#if SCH_IS_CHARTBOOST_ENABLED == true
                                 ,
                                 ChartboostDelegate
#endif

#if SCH_IS_REVMOB_ENABLED == true
                                 ,
                                 RevMobAdsDelegate
#endif

#if SCH_IS_SOCIAL_ENABLED == true
                                 ,
                                 UIPopoverControllerDelegate
#endif

#if SCH_IS_EVERYPLAY_ENABLED == true
                                 ,
                                 EveryplayDelegate
#endif

#if SCH_IS_MOPUB_ENABLED == true
                                 ,
                                 MPAdViewDelegate, MPInterstitialAdControllerDelegate
#endif

#if SCH_IS_ADCOLONY_ENABLED == true
                                 ,
                                 AdColonyDelegate
#endif

#if SCH_IS_VUNGLE_ENABLED == true
                                 ,
                                 VungleSDKDelegate
#endif

#if SCH_IS_GAME_CENTER_ENABLED == true
                                 ,
                                 GKLocalPlayerListener
#endif

                                 > {
    AppController *appController;
    UIView *view;
    RootViewController *localViewController;

#if SCH_IS_GOOGLE_ANALYTICS_ENABLED == true
    id<GAITracker> tracker;
#endif

#if SCH_IS_iADS_ENABLED == true
    ADBannerView *adView;

    BOOL isBannerVisible;
    BOOL isBannerLoaded;
    BOOL isBannerToBeShown;

    int bannerPosition;
#endif

#if SCH_IS_REVMOB_ENABLED == true
    BOOL isRevMobInitialised;
#endif

#if SCH_IS_AD_MOB_ENABLED == true
    GADInterstitial *adMobInterstitial;
    GADBannerView *adMobBottomBanner;
    GADBannerView *adMobTopBanner;
    BOOL isAdMobFullscreenLoaded;
    BOOL isAdMobBottomBannerDisplayed;
    BOOL isAdMobTopBannerDisplayed;
#endif

#if SCH_IS_MOPUB_ENABLED == true
    BOOL isMopubBannerDisplayed;
#endif
}

#if SCH_IS_MOPUB_ENABLED == true
@property(nonatomic, strong) MPAdView *moPubAdView;
@property(nonatomic, strong) MPInterstitialAdController *moPubinterstitial;
#endif

@property(nonatomic, strong) UIPopoverController *popover;

+ (id)instance;

- (void)initialise;

#if SCH_IS_iADS_ENABLED == true
@property(nonatomic) ADBannerShownCallback adBannerShownCallback;
@property(nonatomic) ADBannerHiddenCallback adBannerHiddenCallback;

//-( void )showiAdBanner;
- (void)showiAdBanner:(int)position;
- (void)hideiAdBanner;
- (void)hideiAdBannerPermanently;
#endif

#if SCH_IS_REVMOB_ENABLED == true
- (void)showRevMobBanner;
- (void)hideRevMobBanner;
- (void)showRevMobFullScreenAd;
- (void)showRevMobPopupAd;
#endif

#if SCH_IS_CHARTBOOST_ENABLED == true
- (void)showChartboostFullScreenAd;
- (void)showChartboostMoreApps;
- (void)showChartboostVideoAd;
#endif

#if SCH_IS_SOCIAL_ENABLED == true
- (void)shareLinkViaFacebook:(NSString *)linkUrl
                withImageUrl:(NSString *)imageUrl
                    andTitle:(NSString *)title
              andDescription:(NSString *)description;
- (void)shareViaTwitter:(NSString *)message andImage:(NSString *)imagePath;
- (void)shareWithString:(NSString *)message andImage:(NSString *)imagePath;
#endif

#if SCH_IS_GAME_CENTER_ENABLED == true
@property(nonatomic) BOOL gameCenterEnabled;
@property(nonatomic, copy) NSString *leaderboardIdentifier;
@property(nonatomic) GameCenterChallengeCallback challengeCallback;
@property(nonatomic) GKGameCenterViewController *gameCenterViewController;

- (void)gameCenterLoginWithCompletionHandler:(void (^)())completionHandler;
- (void)gameCenterShowLeaderboard;
- (void)gameCenterShowAchievements;
- (void)gameCenterHideUi;
- (void)gameCenterSubmitScore:(int)scoreNumber andLeaderboard:(NSString *)leaderboardID;
- (void)gameCenterSubmitScore:(int)scoreNumber
                 forChallenge:(GKScoreChallenge *)challenge
        withCompletionHandler:(void (^)())completionHandler;
- (void)gameCenterUnlockAchievement:(NSString *)achievementID andPercentage:(float)percent;
- (void)gameCenterResetPlayerAchievements;

- (void)gameCenterRegisterChallengeCallback:(GameCenterChallengeCallback)callback;

- (void)player:(GKPlayer *)player wantsToPlayChallenge:(GKChallenge *)challenge;

- (void)gameCenterGetFriendsBestScoresForLeaderboard:(NSString *)leaderboardID
                               withCompletionHandler:(void (^)(NSArray *))completionHandler;
#endif

#if SCH_IS_AD_MOB_ENABLED == true
- (void)showAdMobBanner:(int)position;
- (void)hideAdMobBanner:(int)position;
- (void)showAdMobFullscreenAd;
#endif

#if SCH_IS_MOPUB_ENABLED == true
- (void)showMopubBanner;
- (void)hideMopubBanner;

- (void)showMoPubFullscreenAd;
#endif

#if SCH_IS_EVERYPLAY_ENABLED == true
- (void)setupEveryplay;
- (void)showEveryplay;
- (void)recordEveryplayVideo;
- (void)playLastEveryplayVideoRecording;
#endif

#if SCH_IS_GOOGLE_ANALYTICS_ENABLED == true
- (void)setGAScreenName:(NSString *)screenString;
- (void)setGADispatchInterval:(int)dispatchInterval;
- (void)sendGAEvent:(NSString *)category:(NSString *)action:(NSString *)label:(NSNumber *)value;
#endif

#if SCH_IS_ADCOLONY_ENABLED == true
- (void)showV4VCAC:(BOOL)withPreOP andPostOp:(BOOL)withPostOp;
- (void)onAdColonyV4VCReward:(BOOL)success
                currencyName:(NSString *)currencyName
              currencyAmount:(int)amount
                      inZone:(NSString *)zoneID;
#endif

#if SCH_IS_VUNGLE_ENABLED == true
- (void)showV4VCV:(BOOL)isIncentivised;
- (void)vungleSDKwillCloseAdWithViewInfo:(NSDictionary *)viewInfo
                 willPresentProductSheet:(BOOL)willPresentProductSheet;
- (void)vungleSDKwillShowAd;
#endif

@end