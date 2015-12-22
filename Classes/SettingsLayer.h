#ifndef __SETTINGS_LAYER_H__
#define __SETTINGS_LAYER_H__

#include "cocos2d.h"

#include "MenuHelper.h"
#include "IAPPresenter.h"
#include "SoundHelper.h"

class SettingsLayer : public cocos2d::Layer
{
  public:
    SettingsLayer(const SoundSettings &soundSettings);
    virtual bool init();

    static SettingsLayer *create(const SoundSettings &soundSettings);

    void menuItemClicked(int itemIndex);

    void setGotoMainMenuCallback(std::function<void()> callback)
    {
        gotoMainMenuCallback = callback;
    }

    void setGotoAboutPageCallback(std::function<void()> callback)
    {
        gotoAboutPageCallback = callback;
    }

    void setSoundSettingsChangedCallback(std::function<void(const SoundSettings &)> callback)
    {
        soundSettingsChangedCallback = callback;
    }

  private:
    std::function<void()> gotoMainMenuCallback;
    std::function<void()> gotoAboutPageCallback;
    std::function<void(const SoundSettings &)> soundSettingsChangedCallback;

    MenuHelper menuHelper;

    SoundSettings soundSettings;
};

#endif // __SETTINGS_LAYER_H__
