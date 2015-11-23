#include "PostLevelLayer.h"

#include <spine/spine-cocos2dx.h>

#include "Base/MipmapSprite.h"
#include "Menu/MenuLabel.h"

USING_NS_CC;

namespace
{
struct PostAnimationInfo {
    std::string name;
    Vec2 position;
};

std::vector<PostAnimationInfo> happyAnimations = {
    {"post_happy_01.x", {0.5, 0.53}}, {"post_happy_02.x", {0.5, 0.52}}, {"jump01.x", {0.5, 0.52}}};

std::vector<PostAnimationInfo> sadAnimations = {
    {"post_sad_01.x", {0.5, 0.54}}, {"post_sad_02.x", {0.5, 0.56}}, {"post_sad_03.x", {0.5, 0.57}}};

const PostAnimationInfo &getRandomAnimation(bool sad)
{
    auto &v = sad ? sadAnimations : happyAnimations;

    return v[random<int>(0, v.size() - 1)];
}
}

PostLevelLayer::PostLevelLayer(int currentScore, int bestScore, int challengedScore,
                               const std::string &challengedUserName)
    : currentScore(currentScore), bestScore(bestScore), challengedScore(challengedScore),
      challengedUserName(challengedUserName),
      menuHelper({{{0.35, 0.23}, 0.12, "back"},
                  {{0.65, 0.23}, 0.12, "restart"},
                  {{0.44, 0.72}, 0.06, "twitter"},
                  {{0.56, 0.72}, 0.06, "facebook"}},
                 std::bind(&PostLevelLayer::menuItemClicked, this, std::placeholders::_1))
{
}

bool PostLevelLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    int scoreTop = 0, scoreBottom = 0;
    std::string textTop, textBottom;

    if (challengedUserName.empty()) {
        textTop = "YOUR SCORE";
        textBottom = "BEST SCORE";

        scoreTop = currentScore;
        scoreBottom = bestScore;
    } else {
        // TODO: remove when lowercase letters are available in rat.ttf
        std::string name = challengedUserName;
        std::transform(name.begin(), name.end(), name.begin(), ::toupper);

        textBottom = name;
        scoreBottom = challengedScore;

        if (bestScore > challengedScore) {
            textTop = "YOUR BEST SCORE";
            scoreTop = bestScore;
        } else {
            textTop = "YOUR SCORE";
            scoreTop = currentScore;
        }
    }

    addDigitsPanel({0.5, 0.8}, textTop, scoreTop);
    addDigitsPanel({0.5, 0.4}, textBottom, scoreBottom);

    const PostAnimationInfo &info = getRandomAnimation(scoreTop < scoreBottom);
    CCLOG("name:%s", info.name.c_str());
    addAnimation(info.name, info.position);

    return true;
}

PostLevelLayer *PostLevelLayer::create(int currentScore, int bestScore, int challengedScore,
                                       const std::string &challengedUserName)
{
    PostLevelLayer *ret = new (std::nothrow)
        PostLevelLayer(currentScore, bestScore, challengedScore, challengedUserName);
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    } else {
        delete ret;
        ret = nullptr;
        return nullptr;
    }
}

void PostLevelLayer::setRestartLevelCallback(std::function<void()> callback)
{
    restartLevelCallback = callback;
}

void PostLevelLayer::setGotoMainMenuCallback(std::function<void()> callback)
{
    gotoMainMenuCallback = callback;
}

void PostLevelLayer::setShareOnFacebookCallback(std::function<void()> callback)
{
    shareOnFacebookCallback = callback;
}

void PostLevelLayer::setShareOnTwitterCallback(std::function<void()> callback)
{
    shareOnTwitterCallback = callback;
}

void PostLevelLayer::menuItemClicked(int itemIndex)
{
    if (itemIndex == 0 && gotoMainMenuCallback) {
        gotoMainMenuCallback();
    }

    if (itemIndex == 1 && restartLevelCallback) {
        restartLevelCallback();
    }

    if (itemIndex == 2 && shareOnFacebookCallback) {
        shareOnTwitterCallback();
    }

    if (itemIndex == 3 && shareOnFacebookCallback) {
        shareOnFacebookCallback();
    }
}

void PostLevelLayer::addDigitsPanel(const cocos2d::Vec2 &position, const std::string &text,
                                    int score)
{
    auto panel = DigitsPanel::createWithNumberOfDigits(6);
    MenuHelper::positionNode(*panel, position, 0.05);
    addChild(panel, 1);

    addChild(MenuLabel::create(text, position + Vec2(0, 0.0625), 0.03));

    panel->animateToNumber(score);

    auto background = MipmapSprite::create("background/inner03.png");
    MenuHelper::positionNode(*background, position - Vec2(0, 0.01), 0.55);
    addChild(background);
}

void PostLevelLayer::addAnimation(const std::string &name, const cocos2d::Vec2 &position)
{
    auto ratNode = spine::SkeletonAnimation::createWithFile("animations/rat/skeleton.json",
                                                            "animations/rat/skeleton.atlas");
    ratNode->setAnimation(0, name, true);
    ratNode->updateWorldTransform();
    MenuHelper::positionNode(*ratNode, position, 0.12);
    addChild(ratNode);
}
