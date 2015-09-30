#ifndef __GAMECENTER_PLAYERSCORE_H__
#define __GAMECENTER_PLAYERSCORE_H__

#include <string>
#include <vector>

namespace SonarCocosHelper
{

struct GameCenterPlayerScore {
    GameCenterPlayerScore() : score(0), isChallenge(false), isOwnScore(false) {}
    GameCenterPlayerScore(const std::string &playerName, const std::string &leaderboardId,
                          int score, bool isChallenge = false, bool isOwnScore = false)
        : playerName(playerName), leaderboardId(leaderboardId), score(score),
          isChallenge(isChallenge), isOwnScore(isOwnScore)
    {
    }

    std::string playerName;
    std::string leaderboardId;
    int score;
    bool isChallenge;
    bool isOwnScore; // true for local player

    int getLowerScoreBound() const { return isChallenge ? 0 : std::max(0, score - 50); }
};

typedef std::vector<GameCenterPlayerScore> GameCenterPlayersScores;
}

#endif // __GAMECENTER_PLAYERSCORE_H__
