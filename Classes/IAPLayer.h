#ifndef __IAP_LAYER_H__
#define __IAP_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"
#include "IAP/InAppPurchaseHelper.h"
#include "IAPPresenter.h"

class IAPLayer : public cocos2d::Layer, public IAPPresenter
{
  public:
    IAPLayer(const std::string &productId, const cocos2d::Vec2 &buttonPosition, float buttonSize,
             bool restoreProduct);
    virtual bool init();

    static IAPLayer *create(const std::string &productId, const cocos2d::Vec2 &buttonPosition,
                            float buttonSize, bool restoreProduct = false);

    void menuItemClicked(int itemIndex);

    void setPurchaseCompletedCallback(std::function<void()> callback)
    {
        purchaseCompletedCallback = callback;
    }

  public:
    void purchaseAvailable() override;
    void purchaseInProgress() override;
    void purchaseCompleted() override;
    void purchaseCancelled() override;
    void purchaseFailed() override;

  private:
    std::function<void()> purchaseCompletedCallback;

    MenuHelper menuHelper;

    std::string productId;
    bool restoreProduct;

    std::shared_ptr<InAppPurchaseHelper> iapHelper;

    static const int inProgressActionTag = 998877;
};

#endif // __IAP_LAYER_H__
