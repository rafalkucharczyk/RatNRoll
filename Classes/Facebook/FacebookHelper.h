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

    typedef std::function<void(bool loggedIn)> LoginCallback;
    typedef std::function<void(bool liked)> LikeCallback;

    void setLoginCallback(LoginCallback loginCallback);
    void setLikeCallback(LikeCallback likeCallback);

    void showLoginControl(int x, int y, int width);
    void hideLoginControl();

    void showLikeControl(int x, int y, int width, const std::string &objectId);
    void hideLikeControl();

    void logOut();

  private:
    std::shared_ptr<FacebookHelperImpl> impl;
};

#endif // _FACEBOOKHELPER_H_