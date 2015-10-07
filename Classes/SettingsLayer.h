#ifndef __SETTINGS_LAYER_H__
#define __SETTINGS_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"
#include "IAPPresenter.h"
#include "SoundHelper.h"

class SettingsLayer : public cocos2d::Layer, public IAPPresenter
{
  public:
    SettingsLayer(const SoundSettings &soundSettings);
    virtual bool init(std::function<void()> initCallback);

    static SettingsLayer *createNoInit(const SoundSettings &soundSettings);

    void menuItemClicked(int itemIndex);

    void setGotoMainMenuCallback(std::function<void()> callback)
    {
        gotoMainMenuCallback = callback;
    }

    void setPurchaseRequestedCallback(std::function<void()> callback)
    {
        purchaseRequestedCallback = callback;
    }

    void setPurchaseCompletedCallback(std::function<void()> callback)
    {
        purchaseCompletedCallback = callback;
    }

    void setSoundSettingsChangedCallback(std::function<void(const SoundSettings &)> callback)
    {
        soundSettingsChangedCallback = callback;
    }

  public:
    void purchaseAvailable() override;
    void purchaseInProgress() override;
    void purchaseCompleted() override;
    void purchaseCancelled() override;
    void purchaseFailed() override;

  private:
    std::function<void()> gotoMainMenuCallback;
    std::function<void()> purchaseRequestedCallback;
    std::function<void()> purchaseCompletedCallback;
    std::function<void(const SoundSettings &)> soundSettingsChangedCallback;

    MenuHelper menuHelper;

    SoundSettings soundSettings;

    static const int inProgressActionTag = 998877;
};

#endif // __SETTINGS_LAYER_H__
