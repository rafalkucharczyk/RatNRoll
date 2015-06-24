#ifndef __PERMANENTSTORAGE_H__
#define __PERMANENTSTORAGE_H__

#include <json.h>

class PermanentStorage
{
  public:
    PermanentStorage();

    void setBestScore(int score);
    int getBestScore() const;

  private:
    void initWithDefaults();
    void load();
    void save();

  private:
    std::string storageFilePath;

    Json::Value storageJsonData;
};

#endif // __PERMANENTSTORAGE_H__
