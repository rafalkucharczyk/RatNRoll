#include "SoundHelper.h"

#include <string>
#include <map>

#include "SimpleAudioEngine.h"
#include "cocos/base/ccRandom.h"

#include "cocos2d.h"

const float SoundSettings::effectsVolume;
const float SoundSettings::musicVolume;

struct SfxItem {
    std::string namePrefix;
    int count;
};

namespace
{
std::map<LevelCustomization::ItemType, SfxItem> itemType2SfxItem = {
    {LevelCustomization::SLOWDOWN, {"burger", 6}}, {LevelCustomization::HALVE, {"skull", 6}},
    {LevelCustomization::SPEEDUP, {"vial", 7}},    {LevelCustomization::BREAK, {"big_burger", 6}},
    {LevelCustomization::FRENZY, {"frenzy", 7}},   {LevelCustomization::SHIELD, {"helmet", 6}},
    {LevelCustomization::HOVER, {"hover", 7}},
};
}

SoundHelper *SoundHelper::instance = nullptr;

SoundHelper &SoundHelper::getInstance()
{
    if (instance == nullptr) {
        instance = new SoundHelper();
    }

    return *instance;
}

SoundHelper::SoundHelper()
    : effectsEnabled(true), nextEffectTime(std::chrono::system_clock::from_time_t(0))
{
}

void SoundHelper::init(const SoundSettings &soundSettings)
{
    using namespace CocosDenshion;

    SimpleAudioEngine *audioEngine = SimpleAudioEngine::getInstance();

    if (soundSettings.effectsEnabled) {
        audioEngine->resumeAllEffects();
        audioEngine->preloadEffect("sounds/burger01.wav");
        audioEngine->setEffectsVolume(soundSettings.effectsVolume);
    } else {
        audioEngine->stopAllEffects();
    }
    effectsEnabled = soundSettings.effectsEnabled;

    if (soundSettings.musicEnabled) {
        audioEngine->resumeBackgroundMusic();
        if (!audioEngine->isBackgroundMusicPlaying()) {
            audioEngine->playBackgroundMusic("sounds/background01.mp3", true);
        }
        audioEngine->setBackgroundMusicVolume(soundSettings.musicVolume);
    } else {
        audioEngine->stopBackgroundMusic();
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

void SoundHelper::playGameOverEffect() { playOneEffect("fall", 7); }

void SoundHelper::playBestScoreBeatenEffect() { playOneEffect("rec_beaten", 7); }

void SoundHelper::playBestScoreNotBeatenEffect() { playOneEffect("rec_failed", 7); }

void SoundHelper::playShadowPlayerEffect() { playOneEffect("shadow", 5); }

void SoundHelper::playOneEffect(const std::string namePrefix, int maxCount)
{
    if (!effectsEnabled) {
        return;
    }

    using namespace std::chrono;

    if (system_clock::now() < nextEffectTime) {
        return; // previous effect still in progress, skip this one
    }

    int soundId = cocos2d::random(1, maxCount);
    std::string wavFileName = "sounds/" + namePrefix + "0" + std::to_string(soundId) + ".wav";

    float effectDuration =
        CocosDenshion::SimpleAudioEngine::getInstance()->getEffectDuration(wavFileName.c_str());

    effectDuration *= 1.5; // increase the duration to have longer periods between sounds

    nextEffectTime = system_clock::now() + duration_cast<seconds>(duration<float>(effectDuration));

    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(wavFileName.c_str());
}
