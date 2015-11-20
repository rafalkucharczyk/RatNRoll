#include "FacebookHelper.h"

#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKShareKit/FBSDKShareKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>

#import "AppController.h"
#import "RootViewController.h"

@interface IOSFacebookHelper : NSObject {
    UIView *parentView;

    FBSDKLikeControl *likeView;
}

-(void)positionView:(UIView*)view At:(int)x And:(int)y With:(int)width;

-(void)showLikeControlFor:(NSString*)objectId At:(int)x And:(int)y With:(int)width;
-(void)hideLikeControl;

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

-(void)showLikeControlFor:(NSString *)objectId At:(int)x And:(int)y With:(int)width
{
    likeView = [[FBSDKLikeControl alloc] init];
    likeView.objectID = objectId;
    likeView.likeControlStyle = FBSDKLikeControlStyleBoxCount;
    likeView.objectType = FBSDKLikeObjectTypePage;

    [self positionView:likeView At:x And:y With:0];
    [parentView addSubview:likeView];
}

-(void)hideLikeControl
{
    [likeView removeFromSuperview];
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

void FacebookHelper::showLikeControl(int x, int y, int width, const std::string &objectId)
{
    [impl->iosFacebookHelper showLikeControlFor:[NSString stringWithUTF8String:objectId.c_str()]
                                             At:x And:y With:width];
}

void FacebookHelper::hideLikeControl()
{
    [impl->iosFacebookHelper hideLikeControl];
}
