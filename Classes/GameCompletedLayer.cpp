#include "GameCompletedLayer.h"
#include "ToString.h"

#include <spine/spine-cocos2dx.h>
#include "Menu/MenuLabel.h"

USING_NS_CC;

namespace
{
std::vector<std::string> levelCompletedTexts = {"AWESOME!\n\nCHEESE FACTORY\n\nLEVEL COMPLETED!",
                                                "WELL DONE!\n\nGRAVEYARD\n\nLEVEL COMPLETED!",
                                                "AMAZING!\n\nMAGIC HALL\n\nLEVEL COMPLETED!"};
};

GameCompletedLayer::GameCompletedLayer(int levelNumber)
    : menuHelper({{{0.5, 0.2}, 0.1, "confirm"}},
                 std::bind(&GameCompletedLayer::itemClicked, this, std::placeholders::_1)),
      levelNumber(levelNumber)
{
}

GameCompletedLayer *GameCompletedLayer::create(int levelNumber)
{
    GameCompletedLayer *ret = new (std::nothrow) GameCompletedLayer(levelNumber);
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }

    return ret;
}

bool GameCompletedLayer::init()
{
    if (!BasicWhiteLayer::init()) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    levelCompletedParticleNode = ParticleSystemQuad::create("level_completed.plist");
    MenuHelper::positionNode(*levelCompletedParticleNode, {0.5, 0.9}, 0);
    levelCompletedParticleNode->setScale(1.5);
    addChild(levelCompletedParticleNode);

    addChild(MenuLabel::create(levelCompletedTexts[levelNumber - 1], {0.5, 0.8}, 0.03));

    auto ratNode = spine::SkeletonAnimation::createWithFile("animations/rat/skeleton.json",
                                                            "animations/rat/skeleton.atlas");

    ratNode->setAnimation(0, "game_completed_0" + std::to_string(levelNumber) + ".x", true);
    ratNode->updateWorldTransform();
    MenuHelper::positionNode(*ratNode, {0.5, 0.35}, 0.25);
    addChild(ratNode);

    return true;
}

void GameCompletedLayer::itemClicked(int itemIndex)
{
    if (itemIndex == 0 && completionConifrmedCallback) {
        completionConifrmedCallback();
    }
}
