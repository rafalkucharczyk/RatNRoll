#ifdef COCOS2D_DEBUG

#include "TestLayer.h"

#include <iomanip>

#include <spine/spine-cocos2dx.h>

#include "MenuHelper.h"
#include "DigitsPanel.h"

USING_NS_CC;

TestLayer::TestLayer() {}

bool TestLayer::init()
{
    if (!LayerColor::initWithColor(Color4B::WHITE)) {
        return false;
    }

    auto digitsPanel = DigitsPanel::createWithNumber(1072345);
    MenuHelper::positionNode(*digitsPanel, {0.5, 0.5}, 0.03);
    addChild(digitsPanel);

    digitsPanel->setNumber(4727);

    digitsPanel->setNumber(3210);

    return true;
}

#endif // COCOS2D_DEBUG