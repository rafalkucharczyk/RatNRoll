#include "PermanentStorage.h"

#include <cocos/platform/CCFileUtils.h>

#include <fstream>

USING_NS_CC;

PermanentStorage::PermanentStorage()
    : storageFilePath(FileUtils::getInstance()->getWritablePath() + "data.json")
{
    if (FileUtils::getInstance()->isFileExist(storageFilePath)) {
        load();
        if (hasValidFormat()) {
            return;
        }
    }

    initWithDefaults();
    save();
}

void PermanentStorage::setBestScore(int levelNumber, int score)
{
    if (!storageJsonData.isValidIndex(levelNumber)) {
        storageJsonData.resize(levelNumber + 1);
        storageJsonData[levelNumber] = Json::Value(Json::objectValue);
    }

    storageJsonData[levelNumber]["bestScore"] = score;

    save();
}

int PermanentStorage::getBestScore(int levelNumber) const
{
    if (!storageJsonData.isValidIndex(levelNumber)) {
        return 0;
    }

    return storageJsonData[levelNumber]["bestScore"].asInt();
}

bool PermanentStorage::hasValidFormat() const { return storageJsonData.isArray(); }

void PermanentStorage::initWithDefaults() { storageJsonData = Json::Value(Json::arrayValue); }

void PermanentStorage::load()
{
    std::ifstream ifs;
    ifs.open(storageFilePath);

    Json::Reader reader;
    reader.parse(ifs, storageJsonData);

    ifs.close();
}

void PermanentStorage::save()
{
    std::ofstream ofs;
    ofs.open(storageFilePath);

    Json::FastWriter writer;

    ofs << writer.write(storageJsonData);

    ofs.close();
}