#include "PermanentStorage.h"

#include <cocos/base/CCUserDefault.h>
#include "KeychainItem.h"

USING_NS_CC;

namespace
{
std::string getBestScoreKey(int levelNumber) { return "bestScore." + std::to_string(levelNumber); }

typedef std::map<std::string, int> KeychainMapType;

KeychainMapType readJsonData(const std::string &jsonString)
{
    KeychainMapType ret;

    Json::Value jsonValue;
    Json::Reader reader;

    if (reader.parse(jsonString, jsonValue)) {
        Json::Value::Members keys = jsonValue.getMemberNames();

        for (const std::string &key : keys) {
            ret[key] = jsonValue.get(key, 0).asInt();
        }
    }

    return ret;
}

std::string saveJsonData(const KeychainMapType &keychainMap)
{
    Json::FastWriter writer;

    Json::Value jsonValue;
    jsonValue = Json::Value(Json::objectValue);

    for (auto &item : keychainMap) {
        jsonValue[item.first] = item.second;
    }

    return writer.write(jsonValue);
}

const std::string effectsEnabledKey = "effects_enabled";
const std::string musicEnabledKey = "music_enabled";
}

PermanentStorage *PermanentStorage::instance = nullptr;

PermanentStorage &PermanentStorage::getInstance()
{
    if (instance == nullptr) {
        instance = new PermanentStorage();
    }

    return *instance;
}

PermanentStorage::PermanentStorage() {}

void PermanentStorage::setBestScore(int levelNumber, int score)
{
    UserDefault::getInstance()->setIntegerForKey(getBestScoreKey(levelNumber).c_str(), score);
}

int PermanentStorage::getBestScore(int levelNumber) const
{
    return UserDefault::getInstance()->getIntegerForKey(getBestScoreKey(levelNumber).c_str(), 0);
}

void PermanentStorage::setPurchaseState(const std::string &productId, bool bought)
{
    KeychainItem keychainItem;

    KeychainMapType keychainMap = readJsonData(keychainItem.getData());

    keychainMap[productId] = static_cast<int>(bought);

    keychainItem.setData(saveJsonData(keychainMap));
}

bool PermanentStorage::getPurchaseState(const std::string &productId) const
{
    KeychainItem keychainItem;

    KeychainMapType keychainMap = readJsonData(keychainItem.getData());

    return (keychainMap.find(productId) != keychainMap.end()) && keychainMap[productId];
}

void PermanentStorage::setSoundSettings(const SoundSettings &soundSettings)
{
    UserDefault::getInstance()->setBoolForKey(effectsEnabledKey.c_str(),
                                              soundSettings.effectsEnabled);
    UserDefault::getInstance()->setBoolForKey(musicEnabledKey.c_str(), soundSettings.musicEnabled);
}

SoundSettings PermanentStorage::getSoundSettings() const
{
    SoundSettings soundSettings;

    soundSettings.effectsEnabled =
        UserDefault::getInstance()->getBoolForKey(effectsEnabledKey.c_str(), true);
    soundSettings.musicEnabled =
        UserDefault::getInstance()->getBoolForKey(musicEnabledKey.c_str(), true);

    return soundSettings;
}
