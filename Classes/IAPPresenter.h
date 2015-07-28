#ifndef __IAPPRESENTER_H__
#define __IAPPRESENTER_H__

class IAPPresenter
{
  public:
    virtual void purchaseAvailable() = 0;
    virtual void purchaseInProgress() = 0;
    virtual void purchaseCompleted() = 0;
    virtual void purchaseCancelled() = 0;
    virtual void purchaseFailed() = 0;
};

#endif // __IAPPRESENTER_H__
