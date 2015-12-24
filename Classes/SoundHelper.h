#ifndef __SOUND_HELPER_H__
#define __SOUND_HELPER_H__

#include "LevelCustomization.h"

#include "MenuHelper.h"

struct SoundSettings {
  public:
    SoundSettings() : effectsEnabled(true), musicEnabled(true) {}

  public:
    bool effectsEnabled;
    bool musicEnabled;

    constexpr static float effectsVolume = 0.5;
    constexpr static float musicVolume = 0.25;
};

class SoundHelper
{
  public:
    static SoundHelper &getInstance();

    void configure(const SoundSettings &soundSettings);

    void playEffectForItem(LevelCustomization::ItemType itemType);

    void playBestScoreBeatenEffect();
    void playBestScoreNotBeatenEffect();

    void playBackgroundMusic(const std::string &musicFile);
    void playBackgroundMusicCrossfade(const std::string &musicFile, float durationInSeconds);

  private:
    void playOneEffect(const std::string namePrefix, int maxCount);

    SoundSettings currentSettings;

    int currentBackgroundMusicEffectId;
    std::string currentBackgroundMusicAsset;

  private:
    SoundHelper();

    static SoundHelper *instance;
};

#endif // __SOUND_HELPER_H__
