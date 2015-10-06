#include "SettingsLayer.h"

#include "MipmapSprite.h"

#include "SonarFrameworks.h"

USING_NS_CC;

SettingsLayer::SettingsLayer()
    : menuHelper(
          {{{0.5, 0.41},
            0.15,
            "basket",
            {[](Node *node) {},
             [](Node *node) { // purchaseInProgress
                 auto action = RepeatForever::create(RotateBy::create(2, 360));
                 action->setTag(inProgressActionTag);
                 node->runAction(action);
             },
             [](Node *node) { // purchaseCompleted
                 node->stopActionByTag(inProgressActionTag);

                 MenuItemSprite *menuItemSprite = dynamic_cast<MenuItemSprite *>(node);

                 auto sprite = MenuItemButton::createSpriteForPath("basket_full");

                 menuItemSprite->setDisabledImage(sprite);
                 menuItemSprite->setEnabled(false);

                 node->runAction(RotateTo::create(0.2, 0));
             },
             [](Node *node) { // purchaseCancelled / purchaseFailed
                 node->stopActionByTag(inProgressActionTag);

                 FiniteTimeAction *moveLeft =
                     MoveBy::create(0.04, Vec2(-0.05 * node->getContentSize().width, 0));
                 FiniteTimeAction *moveRight =
                     MoveBy::create(0.04, Vec2(0.05 * node->getContentSize().width, 0));
                 node->runAction(Sequence::create(
                     RotateTo::create(0.2, 0),
                     Repeat::create(Sequence::create(moveLeft, moveLeft->reverse(), moveRight,
                                                     moveRight->reverse(), nullptr),
                                    3),
                     nullptr));
             }}},
           {{0.5, 0.58}, 0.15, "sound"},
           {{0.5, 0.75}, 0.15, "music01"},
           {{0.5, 0.2}, 0.1, "back"}},
          std::bind(&SettingsLayer::menuItemClicked, this, std::placeholders::_1))
{
}

bool SettingsLayer::init(std::function<void()> initCallback)
{
    if (!Layer::init()) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    initCallback();

    return true;
}

SettingsLayer *SettingsLayer::createNoInit()
{
    SettingsLayer *ret = new (std::nothrow) SettingsLayer();
    if (ret) {
        ret->autorelease();
        return ret;
    } else {
        delete ret;
        ret = NULL;
        return NULL;
    }
}

void SettingsLayer::menuItemClicked(int itemIndex)
{
    if (itemIndex == 0 && purchaseRequestedCallback) {
        purchaseRequestedCallback();
    }

    if (itemIndex == 3 && gotoMainMenuCallback) {
        gotoMainMenuCallback();
    }
}

void SettingsLayer::purchaseAvailable() {}

void SettingsLayer::purchaseInProgress() { menuHelper.runActionFor(0, 1); }

void SettingsLayer::purchaseCompleted()
{
    menuHelper.runActionFor(0, 2);

    if (purchaseCompletedCallback) {
        purchaseCompletedCallback();
    }
}

void SettingsLayer::purchaseCancelled() { menuHelper.runActionFor(0, 3); }

void SettingsLayer::purchaseFailed() { menuHelper.runActionFor(0, 3); }
