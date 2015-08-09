#ifndef __GAMECENTER_PLAYERSCORE_H__
#define __GAMECENTER_PLAYERSCORE_H__

#include <string>
#include <vector>

namespace SonarCocosHelper
{

struct GameCenterPlayerScore {
    GameCenterPlayerScore() : score(0), isChallenge(false) {}
    GameCenterPlayerScore(const std::string &playerName, const std::string &leaderboardId,
                          int score, bool isChallenge)
        : playerName(playerName), leaderboardId(leaderboardId), score(score),
          isChallenge(isChallenge)
    {
    }

    std::string playerName;
    std::string leaderboardId;
    int score;
    bool isChallenge;

    int getLowerScoreBound() const { return isChallenge ? 0 : std::max(0, score - 50); }
};

typedef std::vector<GameCenterPlayerScore> GameCenterPlayersScores;
}

#endif // __GAMECENTER_PLAYERSCORE_H__
