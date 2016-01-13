#include "SettingsLayer.h"

#include "MipmapSprite.h"

#include "SonarFrameworks.h"

USING_NS_CC;

SettingsLayer::SettingsLayer(const SoundSettings &soundSettings)
    : menuHelper(
          {{{0.35 + 0.01, 0.65 + 0.01},
            0.15,
            "sound",
            {MenuHelper::replaceImage([this]() { return this->soundSettings.effectsEnabled; },
                                      "sound", "sound_disabled")}},
           {{0.65 + 0.01, 0.65 - 0.01},
            0.15,
            "music01",
            {MenuHelper::replaceImage([this]() { return this->soundSettings.musicEnabled; },
                                      "music01", "music01_disabled")}},
           {{0.65 - 0.01, 0.45 - 0.01}, 0.14, "about"},
           {{0.5, 0.2}, 0.1, "back"}},
          std::bind(&SettingsLayer::menuItemClicked, this, std::placeholders::_1)),
      soundSettings(soundSettings)
{
}

bool SettingsLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    return true;
}

SettingsLayer *SettingsLayer::create(const SoundSettings &soundSettings)
{
    SettingsLayer *ret = new (std::nothrow) SettingsLayer(soundSettings);
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    } else {
        delete ret;
        ret = NULL;
        return NULL;
    }
}

void SettingsLayer::menuItemClicked(int itemIndex)
{
    if (itemIndex == 0) {
        soundSettings.effectsEnabled = !soundSettings.effectsEnabled;
        menuHelper.runActionFor(0, 0);

        if (soundSettingsChangedCallback) {
            soundSettingsChangedCallback(soundSettings);
        }
    }

    if (itemIndex == 1) {
        soundSettings.musicEnabled = !soundSettings.musicEnabled;
        menuHelper.runActionFor(1, 0);

        if (soundSettingsChangedCallback) {
            soundSettingsChangedCallback(soundSettings);
        }
    }

    if (itemIndex == 2 && gotoAboutPageCallback) {
        gotoAboutPageCallback();
    }

    if (itemIndex == 3 && gotoMainMenuCallback) {
        gotoMainMenuCallback();
    }
}
