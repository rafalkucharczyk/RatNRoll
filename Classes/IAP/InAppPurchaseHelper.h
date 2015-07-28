#ifndef __INAPPPURCHASEHELPER_H__
#define __INAPPPURCHASEHELPER_H__

#include "InAppPurchase.h"
#include "IAPPresenter.h"

class InAppPurchaseHelper : public InAppPurchaseListener
{
  public:
    InAppPurchaseHelper(const std::string &productId, IAPPresenter &iapPresenter);

    void init();
    void purchaseProduct();

    static bool isPurchased(const std::string &productId);

  public:
    void inAppPurchaseNotificationReceived(int notification, std::string message,
                                           std::string secondaryMessage) override;

  private:
    std::string productId;
    IAPPresenter &iapPresenter;

    InAppPurchase inAppPurchaseEngine;
};

#endif // __INAPPPURCHASEHELPER_H__
