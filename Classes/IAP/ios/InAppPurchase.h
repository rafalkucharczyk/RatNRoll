#ifndef __INAPPPURCHASE_H__
#define __INAPPPURCHASE_H__

#include <string>

class InAppPurchaseListener;

class InAppPurchase
{
  public:
    InAppPurchase();
    ~InAppPurchase();
    void addListener(InAppPurchaseListener *listener_) { listener = listener_; }
    void requestProduct(std::string productID);
    void purchaseProduct();
    void restoreProduct();

    std::string getLocalizedProductDescription();
    std::string getLocalizedProductTitle();
    std::string getProductPriceInLocalCurrency();

    void sendNotification(int notification, std::string message = "",
                          std::string secondaryMessage = "");

    enum NotificationType {
        error,
        productReceived,
        purchasingProduct,
        productPurchased,
        productRestored,
        productPurchaseCancelled
    };

  private:
    InAppPurchaseListener *listener;
};

class InAppPurchaseListener
{
  public:
    virtual ~InAppPurchaseListener() {}
    virtual void inAppPurchaseNotificationReceived(int notification, std::string message = "",
                                                   std::string secondaryMessage = "") = 0;
};

#endif // __INAPPPURCHASE_H__
