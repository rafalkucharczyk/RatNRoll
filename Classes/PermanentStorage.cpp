#include "PermanentStorage.h"

#include <cocos/base/CCUserDefault.h>

USING_NS_CC;

namespace
{
std::string getBestScoreKey(int levelNumber) { return "bestScore." + std::to_string(levelNumber); }
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
