#ifndef __ASSETS_PRELOADER_H__
#define __ASSETS_PRELOADER_H__

#include <vector>
#include <string>

class AssetsPreloader
{
  public:
    static void preload(const std::string &fileName);

    static std::vector<std::string> list();
};

#endif // __ASSETS_PRELOADER_H__
