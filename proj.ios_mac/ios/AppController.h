#import <UIKit/UIKit.h>
#import <iAd/iAd.h>

@class RootViewController;

@interface AppController : NSObject <UIApplicationDelegate> {
}

@property(nonatomic, retain) UIWindow *window;
@property(nonatomic, readonly) RootViewController *viewController;

@end

