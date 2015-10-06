#include "InAppPurchaseHelper.h"
#include "KeychainItem.h"

InAppPurchaseHelper::InAppPurchaseHelper(const std::string &productId, IAPPresenter &iapPresenter)
    : productId(productId), iapPresenter(iapPresenter), isPurchaseAvailable(false)
{
}

void InAppPurchaseHelper::init()
{
    if (isPurchased(productId)) {
        iapPresenter.purchaseAvailable();
        iapPresenter.purchaseCompleted();

        // *** TODO remove for production code ***
        KeychainItem keychainItem;
        keychainItem.setData(""); // clear purchase when entring settings every second time

        return;
    }

    inAppPurchaseEngine.addListener(this);
    inAppPurchaseEngine.requestProduct(productId);
}

void InAppPurchaseHelper::purchaseProduct()
{
    if (isPurchaseAvailable) {
        inAppPurchaseEngine.purchaseProduct();
    } else {
        deferredAction = [this]() { inAppPurchaseEngine.purchaseProduct(); };
    }
}

bool InAppPurchaseHelper::isPurchased(const std::string &productId)
{
    KeychainItem keychainItem;
    return keychainItem.getData() == productId;
}

void InAppPurchaseHelper::inAppPurchaseNotificationReceived(int notification, std::string message,
                                                            std::string secondaryMessage)
{
    if (notification == InAppPurchase::error) {
        CCLOG("InAppPurchase::error %s", message.c_str());
        iapPresenter.purchaseFailed();
    } else if (notification == InAppPurchase::productReceived) {
        CCLOG("InAppPurchase::productReceived %s",
              inAppPurchaseEngine.getLocalizedProductDescription().c_str());
        CCLOG("InAppPurchase::productReceived %s",
              inAppPurchaseEngine.getLocalizedProductTitle().c_str());
        CCLOG("InAppPurchase::productReceived %s",
              inAppPurchaseEngine.getProductPriceInLocalCurrency().c_str());
        iapPresenter.purchaseAvailable();

        isPurchaseAvailable = true;

        if (deferredAction) {
            deferredAction();
        }
    } else if (notification == InAppPurchase::purchasingProduct) {
        CCLOG("InAppPurchase::purchasingProduct");
        iapPresenter.purchaseInProgress();
    } else if (notification == InAppPurchase::productPurchased ||
               notification == InAppPurchase::productRestored) {
        CCLOG("InAppPurchase::productPurchased/productRestored");
        iapPresenter.purchaseCompleted();

        KeychainItem keychainItem;
        keychainItem.setData(productId);
    } else if (notification == InAppPurchase::productPurchaseCancelled) {
        CCLOG("InAppPurchase::productPurchaseCancelled");
        iapPresenter.purchaseCancelled();
    }
}
