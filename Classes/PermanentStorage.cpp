#include "PermanentStorage.h"

#include <cocos/platform/CCFileUtils.h>

#include <fstream>

USING_NS_CC;

PermanentStorage::PermanentStorage()
    : storageFilePath(FileUtils::getInstance()->getWritablePath() + "data.json")
{
    if (!FileUtils::getInstance()->isFileExist(storageFilePath)) {
        initWithDefaults();
        save();
    } else {
        load();
    }
}

void PermanentStorage::setBestScore(int score)
{
    storageJsonData["bestScore"] = score;

    save();
}

int PermanentStorage::getBestScore() const { return storageJsonData["bestScore"].asInt(); }

void PermanentStorage::initWithDefaults()
{
    storageJsonData = Json::Value(Json::objectValue);

    storageJsonData["bestScore"] = 0;
}

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