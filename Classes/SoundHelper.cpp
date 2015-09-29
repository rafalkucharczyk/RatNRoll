#include "SoundHelper.h"

#include <string>
#include <map>

#include "SimpleAudioEngine.h"
#include "cocos/base/ccRandom.h"

struct SfxItem {
    std::string namePrefix;
    int count;
};

namespace
{
std::map<LevelCustomization::ItemType, SfxItem> itemType2SfxItem = {
    {LevelCustomization::SLOWDOWN, {"burger", 6}},
    {LevelCustomization::HALVE, {"skull", 6}},
    {LevelCustomization::SPEEDUP, {"vial", 7}},
    {LevelCustomization::BREAK, {"big_burger", 6}}};
}

SoundHelper *SoundHelper::instance = nullptr;

SoundHelper &SoundHelper::getInstance()
{
    if (instance == nullptr) {
        instance = new SoundHelper();
    }

    return *instance;
}

void SoundHelper::init()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/burger01.wav");
}

void SoundHelper::playEffectForItem(LevelCustomization::ItemType itemType)
{
    auto sfxItem = itemType2SfxItem.find(itemType);

    if (sfxItem == itemType2SfxItem.end()) {
        return;
    }

    int soundId = cocos2d::random(1, sfxItem->second.count);
    std::string wavFileName =
        "sounds/" + sfxItem->second.namePrefix + "0" + std::to_string(soundId) + ".wav";

    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(wavFileName.c_str());
}