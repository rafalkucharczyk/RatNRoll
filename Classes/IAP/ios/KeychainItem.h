#ifndef __KEYCHAINITEM_H__
#define __KEYCHAINITEM_H__

#include <string>

class KeychainItem
{
  public:
    KeychainItem();

    void setData(const std::string &data);
    std::string getData();
};

#endif // __KEYCHAINITEM_H__