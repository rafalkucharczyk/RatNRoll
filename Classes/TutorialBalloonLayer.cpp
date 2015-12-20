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
       "WELCOME TO RAT'N'ROLL\n\n"
       "LET ME SHOW YOU\n"
       "HOW TO PLAY...",
       0.5},
      {"tutorial01.x",
       {0.4, 0.75},
       "ROLL THE WORLD AND KEEP\n"
       "YOURSELF ON TOP OF IT.\n"
       "THE MORE YOU ROLL,\n"
       "THE MORE YOU SCORE!\n\n"
       "OH... AND REMEMBER TO\n"
       "ALWAYS ROLL CLOCKWISE,\n"
       "YOU GET NOTHING OTHERWISE",
       0.4}}},
    {TutorialBalloonLayer::BalloonType::SPEEDUP,
     {{"tutorial02.x",
       {0.65, 0.7},
       "CATCH THE BOTTLES TO\n"
       "SPEED YOU UP!\n\n"
       "YOU GET MORE POINTS\n"
       "WHEN RUNNING FASTER.\n"
       "AND MORE FUN!",
       0.45}}},
    {TutorialBalloonLayer::BalloonType::SLOWDOWN,
     {{"tutorial03.x",
       {0.25, 0.7},
       "AVOID ANY HAMBURGERS,\n"
       "THEY SLOW YOU DOWN!\n\n"
       "YOU GET LESS POINTS THEN...",
       0.45}}},
    {TutorialBalloonLayer::BalloonType::HOVER,
     {{"tutorial04.x",
       {0.25, 0.68},
       "A GOOSE MAKES YOU FLY FOR\n"
       "A LITTLE WHILE,\n"
       "SO YOU CAN ROLL THE WORLD\n"
       "AS FAST AS YOU CAN!\n\n"
       "BUT BE CAREFUL WHEN\n"
       "LANDING, THOUGH",
       0.45}}},
    {TutorialBalloonLayer::BalloonType::HALVE,
     {{"tutorial05.x",
       {0.3, 0.7},
       "ALWAYS STAY CLEAR\n"
       "OF THE SKULLS. THEY DO NOT\n"
       "KILL YOU, BUT THEY\n"
       "TAKE HALF OF YOUR POINTS!\n\n"
       "IT HURTS EVEN MORE...",
       0.45}}},
    {TutorialBalloonLayer::BalloonType::FINAL,
     {{"post_happy_01.x",
       {0.5, 0.7},
       "CONGRATULATIONS!\n"
       "YOU'VE COMPLETED THE TUTORIAL\n\n"
       "NOW YOU'RE READY\n"
       "TO PLUNGE INTO\n"
       "THE FIRST LEVEL",
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
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 232))) {
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
