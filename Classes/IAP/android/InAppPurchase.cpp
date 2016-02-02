#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <string>
#include <cocos2d.h>
#include "InAppPurchase.h"

InAppPurchase::InAppPurchase() { CCLOG("%s: not implemented for android!", __func__); }

InAppPurchase::~InAppPurchase() { CCLOG("%s: not implemented for android!", __func__); }

void InAppPurchase::requestProduct(std::string productID)
{
    CCLOG("%s: not implemented for android!", __func__);
}

void InAppPurchase::purchaseProduct() { CCLOG("%s: not implemented for android!", __func__); }

void InAppPurchase::restoreProduct() { CCLOG("%s: not implemented for android!", __func__); }

std::string InAppPurchase::getLocalizedProductDescription()
{
    CCLOG("%s: not implemented for android!", __func__);
    return "none";
}

std::string InAppPurchase::getLocalizedProductTitle()
{
    CCLOG("%s: not implemented for android!", __func__);
    return "none";
}

std::string InAppPurchase::getProductPriceInLocalCurrency()
{
    CCLOG("%s: not implemented for android!", __func__);
    return "none";
}

void InAppPurchase::sendNotification(int notification, std::string message,
                                     std::string secondaryMessage)
{
    CCLOG("%s: not implemented for android!", __func__);
}
#endif /* CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID */
