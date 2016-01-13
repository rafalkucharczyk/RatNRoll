#include "IAPLayer.h"

USING_NS_CC;

IAPLayer::IAPLayer(const std::string &productId, const cocos2d::Vec2 &buttonPosition,
                   float buttonSize, bool restoreProduct)
    : menuHelper(
          {{buttonPosition,
            buttonSize,
            "basket",
            {MenuHelper::replaceImage([restoreProduct]() { return restoreProduct; },
                                      "restore_purchase", "basket"),
             [](Node *node) { // purchaseInProgress
                 if (node->getActionByTag(inProgressActionTag) == nullptr) {
                     auto action = RepeatForever::create(RotateBy::create(2, 360));
                     action->setTag(inProgressActionTag);
                     node->runAction(action);
                 }
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
             }}}},
          std::bind(&IAPLayer::menuItemClicked, this, std::placeholders::_1)),
      productId(productId), restoreProduct(restoreProduct)
{
}

bool IAPLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    setCascadeOpacityEnabled(true);

    iapHelper.reset(new InAppPurchaseHelper(productId, *this));
    iapHelper->init();

    return true;
}

IAPLayer *IAPLayer::create(const std::string &productId, const cocos2d::Vec2 &buttonPosition,
                           float buttonSize, bool restore)
{
    IAPLayer *ret = new (std::nothrow) IAPLayer(productId, buttonPosition, buttonSize, restore);
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }

    return ret;
}

void IAPLayer::menuItemClicked(int itemIndex)
{
    if (itemIndex == 0) {
        menuHelper.runActionFor(0, 1);

        if (restoreProduct) {
            iapHelper->restoreProduct();
        } else {
            iapHelper->purchaseProduct();
        }
    }
}

void IAPLayer::purchaseAvailable() {}

void IAPLayer::purchaseInProgress() { menuHelper.runActionFor(0, 1); }

void IAPLayer::purchaseCompleted()
{
    menuHelper.runActionFor(0, 2);

    if (purchaseCompletedCallback) {
        purchaseCompletedCallback();
    }
}

void IAPLayer::purchaseCancelled() { menuHelper.runActionFor(0, 3); }

void IAPLayer::purchaseFailed() { menuHelper.runActionFor(0, 3); }
