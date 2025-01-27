#ifndef __INAPPPURCHASEHELPER_H__
#define __INAPPPURCHASEHELPER_H__

#include <functional>

#include "InAppPurchase.h"
#include "IAPPresenter.h"

class InAppPurchaseHelper : public InAppPurchaseListener
{
  public:
    InAppPurchaseHelper(const std::string &productId, IAPPresenter &iapPresenter);

    void init();
    void purchaseProduct();
    void restoreProduct();

    static bool isPurchased(const std::string &productId);

  public:
    void inAppPurchaseNotificationReceived(int notification, std::string message,
                                           std::string secondaryMessage) override;

  private:
    std::string productId;
    IAPPresenter &iapPresenter;

    bool isPurchaseAvailable;
    std::function<void()> deferredAction;

    InAppPurchase inAppPurchaseEngine;
};

#endif // __INAPPPURCHASEHELPER_H__
