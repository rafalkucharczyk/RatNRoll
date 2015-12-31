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
    bool musicSettingsUpdated = currentSettings.musicEnabled != soundSettings.musicEnabled;
    currentSettings = soundSettings;

    if (!musicSettingsUpdated) {
        return;
    }

    if (currentSettings.musicEnabled) {
        playBackgroundMusic(currentBackgroundMusicAsset, true);
    }

    if (!currentSettings.musicEnabled) {
        AudioEngine::stop(currentBackgroundMusicEffectId);
    }
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

void SoundHelper::playBackgroundMusic(const std::string &musicFile, bool restart)
{
    if (!shouldPlayBackgroudMusic(musicFile, restart)) {
        return;
    }

    AudioEngine::stop(currentBackgroundMusicEffectId);
    currentBackgroundMusicEffectId =
        AudioEngine::play2d(getAssetFileName(musicFile), true, currentSettings.musicVolume);
}

void SoundHelper::playBackgroundMusicCrossfade(const std::string &musicFile,
                                               float durationInSeconds, bool restart)
{
    if (!shouldPlayBackgroudMusic(musicFile, restart)) {
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

bool SoundHelper::shouldPlayBackgroudMusic(const std::string &musicFile, bool restart)
{
    if (musicFile.empty()) {
        return false;
    }

    if (musicFile == currentBackgroundMusicAsset && !restart) {
        return false;
    }

    // remember what to play if music is disabled
    currentBackgroundMusicAsset = musicFile;

    if (!currentSettings.musicEnabled) {
        return false;
    }

    return true;
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
