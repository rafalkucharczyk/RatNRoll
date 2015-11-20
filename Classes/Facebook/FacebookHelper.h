#ifndef _FACEBOOKHELPER_H_
#define _FACEBOOKHELPER_H_

#include <string>
#include <functional>
#include <memory>

struct FacebookHelperImpl;

class FacebookHelper
{
  public:
    FacebookHelper();
    ~FacebookHelper();

    void showLikeControl(int x, int y, int width, const std::string &objectId);
    void hideLikeControl();

  private:
    std::shared_ptr<FacebookHelperImpl> impl;
};

#endif // _FACEBOOKHELPER_H_