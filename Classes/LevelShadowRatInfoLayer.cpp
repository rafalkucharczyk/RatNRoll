#include "LevelShadowRatInfoLayer.h"
#include "ToString.h"

#include "Menu/MenuHelper.h"
#include "Menu/MenuLabel.h"

#include "SonarCocosHelper/SonarFrameworks.h"

USING_NS_CC;

LevelShadowRatInfoLayer::LevelShadowRatInfoLayer() : infoNode(nullptr) {}

void LevelShadowRatInfoLayer::showShadowRatInfo(const std::string &playerName, int score, int state)
{
    std::string text;

    switch (state) {
    case -1:
        text = playerName + "\n IS STILL BETTER THAN YOU";
        break;

    case 0:
        text = "BEAT " + playerName + "\nSCORE " + std::to_string(score) + " POINTS!";
        break;

    case 1:
        text = "YOU'VE SCORED MORE THAN\n " + playerName;
        break;
    }

    if (infoNode != nullptr) {
        infoNode->runAction(
            Sequence::create(FadeTo::create(0.25, 0), RemoveSelf::create(), nullptr));
    }

    infoNode = Node::create();
    infoNode->setCascadeOpacityEnabled(true);

    auto label = MenuLabel::create(text, {0.5, 0.82}, 0.03);
    infoNode->addChild(label);

    Sprite *photo = SonarCocosHelper::GameCenter::getImageForPlayer(playerName);

    if (photo) {
        MenuHelper::positionNode(*photo, {0.5, 0.89}, 0.05);
        infoNode->addChild(photo);
    }

    infoNode->setOpacity(0);
    infoNode->runAction(Sequence::create(
        FadeTo::create(0.5, 255), DelayTime::create(2.0), FadeTo::create(0.5, 0),
        CallFunc::create([this]() { this->infoNode = nullptr; }), RemoveSelf::create(), nullptr));

    addChild(infoNode);
}
