/*
 Copyright (C) 2015 Sonar Systems - All Rights Reserved
 You may use, distribute and modify this code under the
 terms of the MIT license
 
 Any external frameworks used have their own licenses and
 should be followed as such.
 */
//
//  IOSCPPHelper.mm
//  Sonar Cocos Helper
//
//  Created by Sonar Systems on 03/03/2015.
//
//

#include "IOSCPPHelper.h"
#import "IOSHelper.h"

static NSString *string2NSString(const __String &str)
{
    return [NSString stringWithCString:str.getCString() encoding:NSUTF8StringEncoding];
}

void IOSCPPHelper::Setup( )
{
    [[IOSHelper instance] initialise];
}

#if SCH_IS_REVMOB_ENABLED == true
void IOSCPPHelper::showRevMobBanner( )
{
    [[IOSHelper instance] showRevMobBanner];
}

void IOSCPPHelper::hideRevMobBanner( )
{
    [[IOSHelper instance] hideRevMobBanner];
}

void IOSCPPHelper::showRevMobFullScreenAd( )
{
    [[IOSHelper instance] showRevMobFullScreenAd];
}

void IOSCPPHelper::showRevMobPopupAd( )
{
    [[IOSHelper instance] showRevMobPopupAd];
}
#endif

#if SCH_IS_iADS_ENABLED == true

namespace {
    std::function<void(bool)> _adShownCallback;
    std::function<void()> _adHiddenCallback;

    void helperAdShownCallback(bool needPause)
    {
        assert(_adShownCallback);

        _adShownCallback(needPause);
    }

    void helperAdHiddenCallback()
    {
        assert(_adHiddenCallback);

        _adHiddenCallback();
    }
}

void IOSCPPHelper::showiAdBanner( int position )
{
    [[IOSHelper instance] showiAdBanner:position];
}

void IOSCPPHelper::showiAdBannerWithCallbacks(
                                       int position,
                                       std::function<void(bool)> adShownCallback,
                                       std::function<void()> adHiddenCallback)
{
    _adShownCallback = adShownCallback;
    _adHiddenCallback = adHiddenCallback;

    [[IOSHelper instance] setAdBannerShownCallback:helperAdShownCallback];
    [[IOSHelper instance] setAdBannerHiddenCallback:helperAdHiddenCallback];
}


void IOSCPPHelper::hideiAdBanner( )
{
    [[IOSHelper instance] hideiAdBannerPermanently];
}
#endif

#if SCH_IS_CHARTBOOST_ENABLED == true
void IOSCPPHelper::showChartboostFullScreenAd( )
{
    [[IOSHelper instance] showChartboostFullScreenAd];
}

void IOSCPPHelper::showChartboostMoreApps( )
{
    [[IOSHelper instance] showChartboostMoreApps];
}

void IOSCPPHelper::showChartboostVideoAd( )
{
    [[IOSHelper instance] showChartboostVideoAd];
}
#endif

#if SCH_IS_SOCIAL_ENABLED == true
void IOSCPPHelper::shareViaFacebook( __String message, __String thumbnailPath )
{
    [[IOSHelper instance] shareViaFacebook:[NSString stringWithCString:message.getCString( ) encoding:NSUTF8StringEncoding]: [NSString stringWithCString:thumbnailPath.getCString( ) encoding:NSUTF8StringEncoding]];
}

void IOSCPPHelper::shareViaTwitter( __String message, __String thumbnailPath )
{
    [[IOSHelper instance] shareViaTwitter:[NSString stringWithCString:message.getCString( ) encoding:NSUTF8StringEncoding]: [NSString stringWithCString:thumbnailPath.getCString( ) encoding:NSUTF8StringEncoding]];
}

void IOSCPPHelper::shareWithString( __String message, __String thumbnailPath )
{
    [[IOSHelper instance] shareWithString:[NSString stringWithCString:message.getCString( ) encoding:NSUTF8StringEncoding]: [NSString stringWithCString:thumbnailPath.getCString( ) encoding:NSUTF8StringEncoding]];
}
#endif

#if SCH_IS_GAME_CENTER_ENABLED == true
namespace {
    GKScoreChallenge *currentChallenge = nil;

    std::function<void(SonarCocosHelper::GameCenterPlayerScore)> gameCenterChallengeCallback;

    std::map<std::string, SonarCocosHelper::GameCenterPlayersScores> friendsBestScores;

    std::map<std::string, std::shared_ptr<Sprite> > friendsPhotos;
    std::shared_ptr<Sprite> placeholderFriendPhoto = nullptr;

    std::string NSString2string(NSString *str)
    {
        return std::string([str UTF8String]);
    }

    void helperChallengeCallback(GKScoreChallenge *challenge)
    {
        assert(gameCenterChallengeCallback);

        NSString *playerName = [[challenge issuingPlayer] alias];
        int score = [[challenge score] value];
        NSString *leaderboardId = [[challenge score] leaderboardIdentifier];

        [challenge retain];

        currentChallenge = challenge;

        gameCenterChallengeCallback(
            SonarCocosHelper::GameCenterPlayerScore(
                NSString2string(playerName), NSString2string(leaderboardId), score, true));
    }

    cocos2d::Sprite *getCCSpriteFromUIImage(UIImage *uiImage)
    {
        NSData *imageData = UIImagePNGRepresentation(uiImage);

        cocos2d::Image *image = new cocos2d::Image();

        image->initWithImageData(static_cast<const Byte*>([imageData bytes]), imageData.length);
        image->autorelease();

        cocos2d::Texture2D *texture = new cocos2d::Texture2D();
        texture->initWithImage(image);
        texture->autorelease();

        return cocos2d::Sprite::createWithTexture(texture);
    }

    UIImage *maskRoundedImage(UIImage *image)
    {
        UIImageView *imageView = [[UIImageView alloc] initWithImage:image];
        CALayer *layer = imageView.layer;

        layer.masksToBounds = YES;
        CGSize size = imageView.bounds.size;
        layer.cornerRadius = CGFloat(MIN(size.width / 2, size.height / 2));

        UIGraphicsBeginImageContext(size);
        [layer renderInContext:UIGraphicsGetCurrentContext()];
        UIImage *roundedImage = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();

        [imageView release];

        return roundedImage;
    }
}

void IOSCPPHelper::gameCenterLogin(std::function<void()> signedInCallback)
{
    [[IOSHelper instance] gameCenterLoginWithCompletionHandler:^{
        signedInCallback();
    }];
}

void IOSCPPHelper::gameCenterShowLeaderboard( )
{
    [[IOSHelper instance] gameCenterShowLeaderboard];
}

void IOSCPPHelper::gameCenterShowAchievements( )
{
    [[IOSHelper instance] gameCenterShowAchievements];
}

void IOSCPPHelper::gameCenterHideUi( )
{
    [[IOSHelper instance] gameCenterHideUi];
}

void IOSCPPHelper::gameCenterSubmitScore( int scoreNumber, __String leaderboardID )
{
    [[IOSHelper instance] gameCenterSubmitScore:scoreNumber
                                 andLeaderboard:[NSString stringWithCString:leaderboardID.getCString( ) encoding:NSUTF8StringEncoding]];
}

void IOSCPPHelper::gameCenterSubmitScoreForChallenge( int scoreNumber )
{
    if (currentChallenge)
    {
        [[IOSHelper instance] gameCenterSubmitScore:scoreNumber forChallenge:currentChallenge
                              withCompletionHandler:^{
                                  [currentChallenge release];
                                  currentChallenge = nil;
                              }];
    }
}

void IOSCPPHelper::gameCenterUnlockAchievement( __String achievementID, float percent )
{
    [[IOSHelper instance] gameCenterUnlockAchievement:[NSString stringWithCString:achievementID.getCString( ) encoding:NSUTF8StringEncoding] andPercentage:percent];
}

void IOSCPPHelper::gameCenterResetPlayerAchievements( )
{
    [[IOSHelper instance] gameCenterResetPlayerAchievements];
}

void IOSCPPHelper::gameCenterRegisterChallengeCallback(std::function<void(SonarCocosHelper::GameCenterPlayerScore)> callback)
{
    gameCenterChallengeCallback = callback;
    [[IOSHelper instance] gameCenterRegisterChallengeCallback:helperChallengeCallback];
}

SonarCocosHelper::GameCenterPlayersScores IOSCPPHelper::gameCenterGetFriendsBestScores(__String leaderboardID)
{
    if (leaderboardID.length() == 0)
    {
        return {};
    }

    [[IOSHelper instance] gameCenterGetFriendsBestScoresForLeaderboard:string2NSString(leaderboardID)
                                                 withCompletionHandler:^(NSArray *friendsScores)
    {
        SonarCocosHelper::GameCenterPlayersScores scores;

        if (!placeholderFriendPhoto)
        {
            UIImage *photo = [UIImage imageNamed:@"gcPlaceholderFriendPhoto.png"];
            Sprite *sprite = getCCSpriteFromUIImage(maskRoundedImage(photo));
            sprite->retain();

            placeholderFriendPhoto = std::shared_ptr<Sprite>(sprite, std::mem_fun(&Sprite::release));
        }

        for (GKScore* score in friendsScores) {
            bool isOwnScore = [[[GKLocalPlayer localPlayer] alias]
                               isEqualToString: [[score player] alias]];

            [[score player] loadPhotoForSize:GKPhotoSizeSmall
                       withCompletionHandler:^(UIImage *photo, NSError *error) {
                if (photo != nil)
                {
                    Sprite *sprite = getCCSpriteFromUIImage(maskRoundedImage(photo));
                    sprite->retain();
                    friendsPhotos[NSString2string([[score player] alias])] =
                        std::shared_ptr<Sprite>(sprite, std::mem_fun(&Sprite::release));;
                }
            }];

            scores.push_back(
                SonarCocosHelper::GameCenterPlayerScore(NSString2string([[score player] alias]),
                                                        leaderboardID.getCString(),
                                                        [score value],
                                                        false,
                                                        isOwnScore));
        }

        friendsBestScores[leaderboardID.getCString()] = scores;
    }];

    return friendsBestScores[leaderboardID.getCString()];
}

void IOSCPPHelper::gameCenterClearCurrentChallenge()
{
    currentChallenge = nullptr;
}

cocos2d::Sprite *IOSCPPHelper::getImageForPlayer(__String playerName)
{
    Sprite *sprite = placeholderFriendPhoto.get();

    if (friendsPhotos.find(playerName.getCString()) != friendsPhotos.end()) {
        sprite = friendsPhotos[playerName.getCString()].get();
    }

    if (sprite) {
        // return cloned object
        return Sprite::createWithSpriteFrame(sprite->displayFrame());
    }

    return nullptr;
}
#endif

#if SCH_IS_AD_MOB_ENABLED == true
void IOSCPPHelper::showAdMobBanner( int position )
{
    [[IOSHelper instance] showAdMobBanner:position];
}

void IOSCPPHelper::hideAdMobBanner( int position )
{
    [[IOSHelper instance] hideAdMobBanner:position];
}

void IOSCPPHelper::showAdMobFullscreenAd( )
{
    [[IOSHelper instance] showAdMobFullscreenAd];
}
#endif

#if SCH_IS_MOPUB_ENABLED == true
void IOSCPPHelper::showMopubBanner( )
{
    [[IOSHelper instance] showMopubBanner];
}

void IOSCPPHelper::hideMopubBanner( )
{
    [[IOSHelper instance] hideMopubBanner];
}

void IOSCPPHelper::showMoPubFullscreenAd( )
{
    [[IOSHelper instance] showMoPubFullscreenAd];
}
#endif


#if SCH_IS_EVERYPLAY_ENABLED == true
void IOSCPPHelper::setupEveryplay( )
{
    [[IOSHelper instance] setupEveryplay];
}

void IOSCPPHelper::showEveryplay( )
{
    [[IOSHelper instance] showEveryplay];
}

void IOSCPPHelper::recordEveryplayVideo( )
{
    [[IOSHelper instance] recordEveryplayVideo];
}

void IOSCPPHelper::playLastEveryplayVideoRecording( )
{
    [[IOSHelper instance] playLastEveryplayVideoRecording];
}
#endif

#if SCH_IS_GOOGLE_ANALYTICS_ENABLED == true
void IOSCPPHelper::setGAScreenName( __String screenName )
{
    [[IOSHelper instance] setGAScreenName:[NSString stringWithCString:screenName.getCString( ) encoding:NSUTF8StringEncoding]];
}

void IOSCPPHelper::setGADispatchInterval( int dispatchInterval )
{
    [[IOSHelper instance] setGADispatchInterval:dispatchInterval];
}

void IOSCPPHelper::sendGAEvent( __String category, __String action, __String label, long value )
{
    [[IOSHelper instance] sendGAEvent:[NSString stringWithCString:category.getCString( ) encoding:NSUTF8StringEncoding]: [NSString stringWithCString:action.getCString( ) encoding:NSUTF8StringEncoding]: [NSString stringWithCString:label.getCString( ) encoding:NSUTF8StringEncoding]: [NSNumber numberWithLong:(value)]];
}
#endif

#if SCH_IS_ADCOLONY_ENABLED == true
void IOSCPPHelper::showVideoAC( bool withPreOp, bool withPostOp )
{
    [[IOSHelper instance] showV4VCAC: withPreOp andPostOp: withPostOp];
}
#endif

#if SCH_IS_VUNGLE_ENABLED == true
void IOSCPPHelper::showVideoVungle( bool isIncentivised )
{
    [[IOSHelper instance] showV4VCV: isIncentivised];
}
#endif

