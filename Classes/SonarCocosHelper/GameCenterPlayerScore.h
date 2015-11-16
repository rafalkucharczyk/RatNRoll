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

    // returns function giving lower score bound depending on current score multiplier
    std::function<int(int)> getLowerScoreBound() const
    {
        if (isChallenge) {
            return [](int) { return 0; };
        }

        int _score = score;

        return
            [_score](int scoreMultiplier) { return std::max(0, _score - 100 * scoreMultiplier); };
    }
};

typedef std::vector<GameCenterPlayerScore> GameCenterPlayersScores;
}

#endif // __GAMECENTER_PLAYERSCORE_H__
