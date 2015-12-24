#include "SoundHelper.h"

#include <string>
#include <map>

#include <audio/include/AudioEngine.h>
#include <cocos/base/ccRandom.h>

#include <thread>

using namespace cocos2d::experimental;

const float SoundSettings::effectsVolume;
const float SoundSettings::musicVolume;

struct SfxItem {
    std::string namePrefix;
    int count;
};

namespace
{
std::map<LevelCustomization::ItemType, SfxItem> itemType2SfxItem = {
    {LevelCustomization::SLOWDOWN, {"burger", 3}}, {LevelCustomization::HALVE, {"skull", 3}},
    {LevelCustomization::SPEEDUP, {"vial", 3}},    {LevelCustomization::BREAK, {"big_burger", 2}},
    {LevelCustomization::FRENZY, {"frenzy", 3}},   {LevelCustomization::SHIELD, {"helmet", 3}},
    {LevelCustomization::HOVER, {"hover", 3}},
};

std::string getAssetFileName(const std::string &asset) { return "sounds/" + asset + ".mp3"; }
}

SoundHelper *SoundHelper::instance = nullptr;

SoundHelper &SoundHelper::getInstance()
{
    if (instance == nullptr) {
        instance = new SoundHelper();
    }

    return *instance;
}

SoundHelper::SoundHelper() : currentBackgroundMusicEffectId(AudioEngine::INVALID_AUDIO_ID) {}

void SoundHelper::configure(const SoundSettings &soundSettings)
{
    const int steps = 10;

    const float durationInSeconds = 0.5;

    const float delay = durationInSeconds / steps;
    const float delta = currentSettings.musicVolume / steps;

    if (soundSettings.musicEnabled != currentSettings.musicEnabled) {
        if (!soundSettings.musicEnabled) {
            std::thread t([=]() {
                for (int i = 1; i <= steps; i++) {
                    AudioEngine::setVolume(currentBackgroundMusicEffectId,
                                           currentSettings.musicVolume - i * delta);
                    usleep(delay * 1E6);
                }
                AudioEngine::pause(currentBackgroundMusicEffectId);
            });

            t.detach();
        } else {
            std::thread t([=]() {
                AudioEngine::resume(currentBackgroundMusicEffectId);

                for (int i = 1; i <= steps; i++) {
                    AudioEngine::setVolume(currentBackgroundMusicEffectId, i * delta);
                    usleep(delay * 1E6);
                }
            });

            t.detach();
        }
    }

    currentSettings = soundSettings;
}

void SoundHelper::playEffectForItem(LevelCustomization::ItemType itemType)
{
    auto sfxItem = itemType2SfxItem.find(itemType);

    if (sfxItem == itemType2SfxItem.end()) {
        return;
    }

    playOneEffect(sfxItem->second.namePrefix, sfxItem->second.count);
}

void SoundHelper::playBestScoreBeatenEffect() { playOneEffect("rec_beaten", 7); }

void SoundHelper::playBestScoreNotBeatenEffect() { playOneEffect("rec_failed", 7); }

void SoundHelper::playBackgroundMusic(const std::string &musicFile)
{
    if (musicFile == currentBackgroundMusicAsset) {
        return;
    }

    AudioEngine::stop(currentBackgroundMusicEffectId);
    currentBackgroundMusicAsset = musicFile;
    currentBackgroundMusicEffectId = AudioEngine::play2d(getAssetFileName(musicFile), true, 0);

    if (!currentSettings.musicEnabled) {
        AudioEngine::pause(currentBackgroundMusicEffectId);
    } else {
        AudioEngine::setVolume(currentBackgroundMusicEffectId, currentSettings.musicVolume);
    }
}

void SoundHelper::playBackgroundMusicCrossfade(const std::string &musicFile,
                                               float durationInSeconds)
{
    if (!currentSettings.musicEnabled) {
        playBackgroundMusic(musicFile);
        return;
    }

    if (musicFile == currentBackgroundMusicAsset) {
        return;
    }

    std::thread t([this, musicFile, durationInSeconds]() {
        const int steps = 10;

        float delay = durationInSeconds / steps;
        float delta = currentSettings.musicVolume / steps;

        int newBackgroundMusicId = AudioEngine::play2d(getAssetFileName(musicFile), true, 0);

        for (int i = 1; i <= steps; i++) {
            AudioEngine::setVolume(currentBackgroundMusicEffectId,
                                   currentSettings.musicVolume - i * delta);
            AudioEngine::setVolume(newBackgroundMusicId, i * delta);
            usleep(delay * 1E6);
        }
        AudioEngine::stop(currentBackgroundMusicEffectId);

        currentBackgroundMusicEffectId = newBackgroundMusicId;
        currentBackgroundMusicAsset = musicFile;
    });

    t.detach();
}

void SoundHelper::playOneEffect(const std::string namePrefix, int maxCount)
{
    if (!currentSettings.effectsEnabled) {
        return;
    }

    int soundId = cocos2d::random(1, maxCount);
    std::string wavFileName = "sounds/" + namePrefix + "0" + std::to_string(soundId) + ".wav";

    AudioEngine::play2d(wavFileName, false, currentSettings.effectsVolume);
}
