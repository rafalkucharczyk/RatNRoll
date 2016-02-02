#include <cocos2d.h>
#include "PermanentStorage.h"
#include "ToString.h"

#include <cocos/base/CCUserDefault.h>
#include "KeychainItem.h"

USING_NS_CC;

namespace
{
std::string getBestScoreKey(int levelNumber) { return "bestScore." + std::to_string(levelNumber); }

PermanentStorage::CustomDataMap readJsonData(const std::string &jsonString)
{
    PermanentStorage::CustomDataMap ret;

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

std::string saveJsonData(const PermanentStorage::CustomDataMap &customDataMap)
{
    Json::FastWriter writer;

    Json::Value jsonValue;
    jsonValue = Json::Value(Json::objectValue);

    for (auto &item : customDataMap) {
        jsonValue[item.first] = item.second;
    }

    return writer.write(jsonValue);
}

const std::string effectsEnabledKey = "effects_enabled";
const std::string musicEnabledKey = "music_enabled";
const std::string unlockedAchievementsKey = "unlocked_achievements";
const std::string achievementsStateKey = "achievements_state";
const std::string keychainLikingKey = "facebook_like";
const std::string tutorialEnteredKey = "tutorial_entered";
const std::string cheeseFactoryEnteredKey = "cheese_factory_entered";
const std::string tutorialStageKey = "tutorial_stage";
const std::string thresholdScore = "threshold_score";
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
    saveToKeychain(productId, bought);
}

bool PermanentStorage::getPurchaseState(const std::string &productId) const
{
    return readFromKeychain(productId);
}

void PermanentStorage::setLikingState(bool liked) { saveToKeychain(keychainLikingKey, liked); }

bool PermanentStorage::getLikingState() const { return readFromKeychain(keychainLikingKey); }

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

void PermanentStorage::setUnlockedAchievements(const CustomDataMap &achievements)
{
    UserDefault::getInstance()->setStringForKey(unlockedAchievementsKey.c_str(),
                                                saveJsonData(achievements));
}

PermanentStorage::CustomDataMap PermanentStorage::getUnlockedAchievements() const
{
    return readJsonData(
        UserDefault::getInstance()->getStringForKey(unlockedAchievementsKey.c_str()));
}

void PermanentStorage::setAchievementTrackerState(const AchievementTracker::State &state)
{
    int i = 0;
    CustomDataMap data;

    auto make_key = [](int i) { return std::string(1, 'a' + i); };

    for (i = 0; i < LevelCustomization::ITEM_TYPE_MAX; i++) {
        data[make_key(i)] = state.itemsCount[i];
    }

    data[make_key(++i)] = state.tutorialEnteredCount;

    data[make_key(++i)] = state.sequence.first;
    data[make_key(++i)] = state.sequence.second;

    data[make_key(++i)] = state.gameOverCount;

    UserDefault::getInstance()->setStringForKey(achievementsStateKey.c_str(), saveJsonData(data));
}

AchievementTracker::State PermanentStorage::getAchievementTrackerState() const
{
    AchievementTracker::State state;
    CustomDataMap data =
        readJsonData(UserDefault::getInstance()->getStringForKey(achievementsStateKey.c_str()));

    if (data.size() == 0) {
        return state;
    }

    auto i = data.begin();

    for (int j = 0; j < LevelCustomization::ITEM_TYPE_MAX; j++, i++) {
        state.itemsCount[j] = i->second;
    }

    state.tutorialEnteredCount = i++->second;

    state.sequence.first = static_cast<LevelCustomization::ItemType>(i++->second);
    state.sequence.second = i++->second;

    state.gameOverCount = i++->second;

    assert(i == data.end());

    return state;
}

void PermanentStorage::setTutorialEntered(bool entered)
{
    UserDefault::getInstance()->setBoolForKey(tutorialEnteredKey.c_str(), entered);
}

bool PermanentStorage::getTutorialEntered() const
{
    return UserDefault::getInstance()->getBoolForKey(tutorialEnteredKey.c_str());
}

void PermanentStorage::setCheeseFactoryEntered(bool entered)
{
    UserDefault::getInstance()->setBoolForKey(cheeseFactoryEnteredKey.c_str(), entered);
}

bool PermanentStorage::getCheeseFactoryEntered() const
{
    return UserDefault::getInstance()->getBoolForKey(cheeseFactoryEnteredKey.c_str());
}

void PermanentStorage::setTutorialStage(int ballonType)
{
    UserDefault::getInstance()->setIntegerForKey(tutorialStageKey.c_str(), ballonType);
}

int PermanentStorage::getTutorialStage() const
{
    return UserDefault::getInstance()->getIntegerForKey(tutorialStageKey.c_str(), -1);
}

void PermanentStorage::setScoreThresholdForLevel(int levelNumber, int score)
{
    CustomDataMap data =
        readJsonData(UserDefault::getInstance()->getStringForKey(thresholdScore.c_str()));

    data[std::to_string(levelNumber)] = score;

    UserDefault::getInstance()->setStringForKey(thresholdScore.c_str(), saveJsonData(data));
}

int PermanentStorage::getScoreThresholdForLevel(int levelNumber) const
{
    CustomDataMap data =
        readJsonData(UserDefault::getInstance()->getStringForKey(thresholdScore.c_str()));

    if (data.find(std::to_string(levelNumber)) != data.end()) {
        return data[std::to_string(levelNumber)];
    }

    return 0;
}

void PermanentStorage::saveToKeychain(const std::string &keychainItemKey, bool value)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    KeychainItem keychainItem;

    CustomDataMap keychainMap = readJsonData(keychainItem.getData());

    keychainMap[keychainItemKey] = static_cast<int>(value);

    keychainItem.setData(saveJsonData(keychainMap));
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    CCLOG("%s: not implemented for android!", __func__);
#endif
}

bool PermanentStorage::readFromKeychain(const std::string &keychainItemKey) const
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    KeychainItem keychainItem;

    CustomDataMap keychainMap = readJsonData(keychainItem.getData());

    return (keychainMap.find(keychainItemKey) != keychainMap.end()) && keychainMap[keychainItemKey];
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    CCLOG("%s: not implemented for android!", __func__);
    return false;
#endif
}
