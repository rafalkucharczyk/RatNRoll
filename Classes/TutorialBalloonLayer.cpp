#include "TutorialBalloonLayer.h"

#include "MipmapSprite.h"
#include "Menu/MenuLabel.h"

USING_NS_CC;

namespace
{
std::map<TutorialBalloonLayer::BalloonType, std::list<TutorialScreen>> tutorialScreens = {
    {TutorialBalloonLayer::BalloonType::WELCOME,
     {{"tutorial00.x",
       {0.5, 0.7},
       "HELLO!\n\n"
       "RULES OF THE GAME\n"
       "ARE SIMPLE...",
       0.5},
      {"tutorial01.x",
       {0.4, 0.75},
       "ROLL THE GLOBE AND\n"
       "BALANCE YOURSELF\n"
       "ON TOP OF IT.\n\n"
       "YOU SCORE POINTS WHEN\n"
       "ROLLING CLOCKWISE!",
       0.4}}},
    {TutorialBalloonLayer::BalloonType::SPEEDUP,
     {{"tutorial02.x",
       {0.65, 0.7},
       "YOU SCORE MORE POINTS\n"
       "WHEN RUNNING FASTER.\n\n"
       "CATCH THE BOTTLES TO\n"
       "SPEED YOU UP!",
       0.45}}},
    {TutorialBalloonLayer::BalloonType::SLOWDOWN,
     {{"tutorial03.x",
       {0.25, 0.7},
       "AVOID ANY HAMBURGERS,\n"
       "THEY SLOW YOU DOWN!",
       0.45}}},
    {TutorialBalloonLayer::BalloonType::HOVER,
     {{"tutorial04.x",
       {0.25, 0.68},
       "A BIRD MAKES YOU FLY FOR\n"
       "FOR A WHILE,\n"
       "SO YOU CAN ROLL\n"
       "THE GLOBE AS FAST\n"
       "AS YOU CAN",
       0.45}}},
    {TutorialBalloonLayer::BalloonType::HALVE,
     {{"tutorial05.x",
       {0.3, 0.7},
       "ALWAYS STAY CLEAR\n"
       "OF THE SKULLS.\n"
       "THEY TAKE HALF\n"
       "OF YOUR POINTS!",
       0.45}}},
    {TutorialBalloonLayer::BalloonType::FINAL,
     {{"post_happy_01.x",
       {0.5, 0.7},
       "CONGRATULATIONS!\n\n"
       "YOU'VE COMPLETED\n"
       "THE TUTORIAL",
       0.45}}}};

const int screenLayerTag = 123;
}

TutorialBalloonLayer::TutorialBalloonLayer(BalloonType balloonType,
                                           std::function<void()> closeCallback)
    : currentTutorialScreens(tutorialScreens[balloonType]), closeCallback(closeCallback),
      menuHelper({{{0.5, 0.2},
                   0.1,
                   "play",
                   {[this, balloonType](Node *node) {
                       MenuHelper::removeTransparency()(node);

                       if (balloonType == TutorialBalloonLayer::BalloonType::FINAL) {
                           MenuItemSprite *menuItemSprite = dynamic_cast<MenuItemSprite *>(node);

                           auto sprite = MenuItemButton::createSpriteForPath("confirm");

                           menuItemSprite->setNormalImage(sprite);
                       }
                   }}}},
                 std::bind(&TutorialBalloonLayer::itemClicked, this, std::placeholders::_1))
{
}

TutorialBalloonLayer *TutorialBalloonLayer::create(BalloonType balloonType,
                                                   std::function<void()> closeCallback)
{
    TutorialBalloonLayer *ret = new (std::nothrow) TutorialBalloonLayer(balloonType, closeCallback);
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return NULL;
}

bool TutorialBalloonLayer::init()
{
    if (!BasicWhiteLayer::init()) {
        return false;
    }

    auto menu = menuHelper.getMenu();
    addChild(menu);

    EventListenerTouchOneByOne *listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [&](Touch *touch, Event *event) { return true; };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,
                                                                                          this);
    addDigitsPanel();
    createScreen(currentTutorialScreens.front());
    return true;
}

void TutorialBalloonLayer::onExit()
{
    Layer::onExit();

    closeCallback();
}

void TutorialBalloonLayer::addDigitsPanel()
{
    digitsPanelBackground = MipmapSprite::create("earth/inner03.png");
    digitsPanelBackground->setVisible(false);
    addChild(digitsPanelBackground);
    MenuHelper::positionNode(*digitsPanelBackground, {0.7, 0.595}, 0.36);

    digitsPanel = DigitsPanel::createWithNumberOfDigits(4);
    digitsPanel->setVisible(false);
    addChild(digitsPanel);
    MenuHelper::positionNode(*digitsPanel, {0.7, 0.6}, 0.04);
}

void TutorialBalloonLayer::createScreen(const TutorialScreen &screen)
{
    // remove old content
    Layer *layer = dynamic_cast<Layer *>(getChildByTag(screenLayerTag));
    if (layer) {
        removeChild(layer);
    }

    // create new
    layer = Layer::create();
    layer->setTag(screenLayerTag);
    layer->setCascadeOpacityEnabled(true);

    auto menuLabel = MenuLabel::create(screen.text, {0.5, screen.textYPosition}, 0.03);
    menuLabel->setCascadeOpacityEnabled(true);
    menuLabel->setOpacity(0);

    layer->addChild(menuLabel);
    menuLabel->runAction(FadeIn::create(1.0));

    animationNode = spine::SkeletonAnimation::createWithFile("animations/rat/skeleton.json",
                                                             "animations/rat/skeleton.atlas");

    animationNode->setAnimation(0, screen.animationName, true);
    animationNode->updateWorldTransform();
    animationNode->setEventListener([this](int trackIndex, spEvent *event) {
        if (std::string(event->data->name) == "score") {
            digitsPanel->setVisible(true);
            digitsPanelBackground->setVisible(true);

            std::string mode((event->stringValue != nullptr) ? event->stringValue : "");
            int score = event->intValue;

            if (mode == "instant") {
                digitsPanel->setNumber(score);
            } else {
                digitsPanel->animateToNumber(score);
            }
        }
    });

    MenuHelper::positionNode(*animationNode, screen.animationPosition, 0.15);
    layer->addChild(animationNode);

    addChild(layer);
}

void TutorialBalloonLayer::itemClicked(int itemIndex)
{
    currentTutorialScreens.pop_front();

    if (currentTutorialScreens.empty()) {
        removeFromParentAndCleanup(true);
    } else {
        createScreen(currentTutorialScreens.front());
    }
}
