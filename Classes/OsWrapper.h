#ifndef __OS_WRAPPER__
#define __OS_WRAPPER__

#include <functional>
#include <vector>
#include "GameCenterPlayerScore.h"
#include "MenuHelper.h"

namespace OsWrapper
{

bool isSignedIn();
void signIn(std::function<void()> signedInCallback);
/* itemId corresponds to position of button in vector<ButtonConfig>
 * returned from getInitMenuLayout() which triggered the action. */
void showLeaderboards(int itemId);
void registerChallengeCallback(
    std::function<void(SonarCocosHelper::GameCenterPlayerScore)> callback);
SonarCocosHelper::GameCenterPlayersScores getFriendsBestScores(const std::string &leaderboardID);

void showAdBannerWithCallbacks(int position, std::function<void(bool)> adShownCallback,
                               std::function<void()> adHiddenCallback);
std::vector<ButtonConfig> getInitMenuLayout();
};

#endif /*  __OS_WRAPPER__ */
