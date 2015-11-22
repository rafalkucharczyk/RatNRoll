#ifndef __SOUND_HELPER_H__
#define __SOUND_HELPER_H__

#include "LevelCustomization.h"

#include "MenuHelper.h"

#include <chrono>

struct SoundSettings {
  public:
    SoundSettings() : effectsEnabled(true), musicEnabled(true) {}

  public:
    bool effectsEnabled;
    bool musicEnabled;

    constexpr static float effectsVolume = 0.5;
    constexpr static float musicVolume = 0.1;
};

class SoundHelper
{
  public:
    static SoundHelper &getInstance();

    void init(const SoundSettings &soundSettings);

    void playEffectForItem(LevelCustomization::ItemType itemType);

    void playGameOverEffect();
    void playBestScoreBeatenEffect();
    void playBestScoreNotBeatenEffect();
    void playShadowPlayerEffect();

  private:
    void playOneEffect(const std::string namePrefix, int maxCount);

    bool effectsEnabled;

    std::chrono::time_point<std::chrono::system_clock> nextEffectTime;

  private:
    SoundHelper();

    static SoundHelper *instance;
};

#endif // __SOUND_HELPER_H__
