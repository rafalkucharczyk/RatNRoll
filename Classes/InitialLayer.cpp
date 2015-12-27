#include "InitialLayer.h"

#include "AssetsPreloader.h"

USING_NS_CC;

InitialLayer::InitialLayer()
    : menuHelper(
          {{{0.5, 0.625}, 0.35, "play"},
           {{0.35, 0.25}, 0.125, "settings"},
           {{0.65, 0.25},
            0.125,
            "gamecenter",
            {[](Node *node) {},

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
             }}}},
          std::bind(&InitialLayer::itemClicked, this, std::placeholders::_1))
{
}

bool InitialLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    addChild(menuHelper.getMenu());

    return true;
}

void InitialLayer::itemClicked(int itemIndex)
{
    if (itemIndex == 0 && playCallback) {
        playCallback();
    } else if (itemIndex == 1 && settingsCallback) {
        settingsCallback();
    } else if (itemIndex == 2 && gameCenterIsSignedInCallback && gameCenterActionCallback) {
        bool signedIn = gameCenterIsSignedInCallback();

        if (!signedIn) {
            menuHelper.runActionFor(2, 1);
        }

        gameCenterActionCallback(signedIn);
    }
}
