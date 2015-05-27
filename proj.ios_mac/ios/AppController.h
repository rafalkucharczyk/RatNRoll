#import <UIKit/UIKit.h>
#import <iAd/iAd.h>

@class RootViewController;

@interface AppController : NSObject <UIApplicationDelegate, ADBannerViewDelegate> {
    UIWindow *window;

    ADBannerView *adView;
    bool bannerIsVisible;
}

@property(nonatomic, readonly) RootViewController* viewController;

@property (nonatomic, retain) ADBannerView *adView;
@property bool bannerIsVisible;

@end

