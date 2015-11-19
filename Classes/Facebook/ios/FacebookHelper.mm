#include "FacebookHelper.h"

#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKShareKit/FBSDKShareKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>

#import "AppController.h"
#import "RootViewController.h"

@interface IOSFacebookHelper : NSObject <FBSDKLoginButtonDelegate> {
    UIView *parentView;

    FBSDKLoginButton *loginView;
    FBSDKLikeControl *likeView;
}

@property FacebookHelper::LoginCallback loginCallback;
@property FacebookHelper::LikeCallback likeCallback;

-(void)positionView:(UIView*)view At:(int)x And:(int)y With:(int)width;

-(void)loginButton:(FBSDKLoginButton *)loginButton
didCompleteWithResult:(FBSDKLoginManagerLoginResult *)result
             error:(NSError *)error;
-(void)loginButtonDidLogOut:(FBSDKLoginButton *)loginButton;

-(void)showLoginControlAt:(int)x And:(int)y With:(int)width;
-(void)hideLoginControl;

-(void)showLikeControlFor:(NSString*)objectId At:(int)x And:(int)y With:(int)width;
-(void)hideLikeControl;

-(void)isLiked:(NSString*)objectId;

-(void)likeViewValueChanged;

-(void)logOut;

@end

@implementation IOSFacebookHelper

-(id)init
{
    if (self = [super init])
    {
        AppController *appController = (AppController *)[[UIApplication sharedApplication] delegate];
        parentView = appController.viewController.view;
    }

    return self;
}

-(void)positionView:(UIView *)view At:(int)x And:(int)y With:(int)width
{
    view.center = CGPointMake(x, y);

    if (width == 0)
    {
        return;
    }

    CGRect frame = [view frame];

    frame.origin.x -= (width - frame.size.width) / 2;
    frame.size.width = width;
    [view setFrame:frame];
}

-(void)loginButton:(FBSDKLoginButton *)loginButton
didCompleteWithResult:(FBSDKLoginManagerLoginResult *)result
             error:(NSError *)error
{
    if (error == nil && _loginCallback)
    {
        _loginCallback(![result isCancelled]);
    }
}

-(void)loginButtonDidLogOut:(FBSDKLoginButton *)loginButton
{
    // intentionally empty
}

-(void)showLoginControlAt:(int)x And:(int)y With:(int)width
{
    FBSDKAccessToken *token = [FBSDKAccessToken currentAccessToken];

    if ([token hasGranted: @"publish_actions"] &&
        [token hasGranted: @"user_likes"]) {
        if (_loginCallback) {
            _loginCallback(YES);
            return;
        }
    }

    loginView = [[FBSDKLoginButton alloc] init];

    loginView.delegate = self;
    loginView.publishPermissions = [NSArray arrayWithObjects: @"publish_actions", nil];
    loginView.readPermissions = [NSArray arrayWithObjects: @"user_likes", nil];

    [self positionView:loginView At:x And:y With:width];
    [parentView addSubview:loginView];
}

-(void)hideLoginControl
{
    [loginView removeFromSuperview];
}

-(void)showLikeControlFor:(NSString *)objectId At:(int)x And:(int)y With:(int)width
{
    [self isLiked:objectId]; // run background check

    likeView = [[FBSDKLikeControl alloc] init];
    likeView.objectID = objectId;
    likeView.likeControlStyle = FBSDKLikeControlStyleBoxCount;
    likeView.objectType = FBSDKLikeObjectTypePage;
    // there is no delegate for FBSDKLikeControl, so we can only hook to event sent when
    // button's UI changes and check for liking status with Graph API, *sigh*...
    [likeView addTarget:self action:@selector(likeViewValueChanged)
       forControlEvents:UIControlEventValueChanged];
    [self positionView:likeView At:x And:y With:0];
    [parentView addSubview:likeView];
}

-(void)hideLikeControl
{
    [likeView removeFromSuperview];
}

-(void)isLiked:(NSString *)objectId
{
    NSString *graphPath = [NSString stringWithFormat:@"me/likes/%@", objectId];

    FBSDKGraphRequest *request = [[FBSDKGraphRequest alloc]
                                  initWithGraphPath:graphPath
                                  parameters:nil
                                  HTTPMethod:@"GET"];

     [request startWithCompletionHandler:^(FBSDKGraphRequestConnection *connection, id result,
                                           NSError *error) {
         if (error == nil)
         {
             BOOL liked = [[result valueForKey:@"data"] count] != 0;

             if (_likeCallback)
             {
                 _likeCallback(liked);
             }
         }
     }];
}

-(void)likeViewValueChanged
{
    [self isLiked:likeView.objectID];
}

-(void)logOut
{
    [[FBSDKLoginManager new] logOut];
}

@end

struct FacebookHelperImpl
{
    IOSFacebookHelper *iosFacebookHelper;
};

FacebookHelper::FacebookHelper() : impl(new FacebookHelperImpl)
{
    impl->iosFacebookHelper = [[IOSFacebookHelper alloc] init];
}

FacebookHelper::~FacebookHelper()
{
    [impl->iosFacebookHelper release];
    impl->iosFacebookHelper = nil;
}

void FacebookHelper::setLoginCallback(LoginCallback loginCallback)
{
    impl->iosFacebookHelper.loginCallback = loginCallback;
}

void FacebookHelper::setLikeCallback(LikeCallback likeCallback)
{
    impl->iosFacebookHelper.likeCallback = likeCallback;
}

void FacebookHelper::showLoginControl(int x, int y, int width)
{
    [impl->iosFacebookHelper showLoginControlAt:x And:y With:width];
}

void FacebookHelper::hideLoginControl()
{
    [impl->iosFacebookHelper hideLoginControl];
}

void FacebookHelper::showLikeControl(int x, int y, int width, const std::string &objectId)
{
    [impl->iosFacebookHelper showLikeControlFor:[NSString stringWithUTF8String:objectId.c_str()]
                                             At:x And:y With:width];
}

void FacebookHelper::hideLikeControl()
{
    [impl->iosFacebookHelper hideLikeControl];
}

void FacebookHelper::logOut()
{
    [impl->iosFacebookHelper logOut];
}
