#ifndef __PERMANENTSTORAGE_H__
#define __PERMANENTSTORAGE_H__

#include <json.h>

class PermanentStorage
{
  public:
    static PermanentStorage &getInstance();

    void setBestScore(int levelNumber, int score);
    int getBestScore(int levelNumber) const;

    void setPurchaseState(const std::string &productId, bool bought);
    bool getPurchaseState(const std::string &productId) const;

  private:
    PermanentStorage();

    static PermanentStorage *instance;
};

#endif // __PERMANENTSTORAGE_H__
