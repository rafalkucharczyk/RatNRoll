#include "AppDelegate.h"

#include "GameFlow.h"
#include "SoundHelper.h"

#include "TransitionSlideChildren.h"

#include "SonarFrameworks.h"

#include <audio/include/AudioEngine.h>

USING_NS_CC;

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {}

// if you want a different context,just modify the value of glContextAttrs
// it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributions,now can only set six attributions:
    // red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages,
// don't modify or remove this function
static int register_all_packages()
{
    return 0; // flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching()
{
    auto director = Director::getInstance();

    // initialize director
    auto glview = director->getOpenGLView();
    if (!glview) {
        glview = GLViewImpl::create("RatNRoll");
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    register_all_packages();

    SonarCocosHelper::IOS::Setup();
    if (!GameFlow::getInstance().iapPurchaseCompleted()) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        SonarCocosHelper::iAds::showiAdBannerWithCallbacks(
            SonarCocosHelper::eBottom,
            [](bool /* stillActive */) { GameFlow::getInstance().pauseGame(); },
            [] { GameFlow::getInstance().resumeGame(); });

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        SonarCocosHelper::AdMob::showBannerAd(SonarCocosHelper::eBottom);
#endif /* (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) */
    }

    SoundHelper::getInstance().configure(GameFlow::getInstance().getSoundSettings());
    SoundHelper::getInstance().playBackgroundMusic("cheese_factory");

    auto scene = GameFlow::getInstance().createPreloadingScene();

    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone
// call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    experimental::AudioEngine::pauseAll();

    Director::getInstance()->stopAnimation();

    if (!GameFlow::getInstance().iapPurchaseCompleted()) {
        SonarCocosHelper::iAds::hideiAdBanner();
    }
    GameFlow::getInstance().pauseGame();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    experimental::AudioEngine::resumeAll();

    Director::getInstance()->startAnimation();

    if (!GameFlow::getInstance().iapPurchaseCompleted()) {
        SonarCocosHelper::iAds::showiAdBanner(SonarCocosHelper::eBottom);
    }
}
