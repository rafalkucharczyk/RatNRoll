#include "LevelLayer.h"

#include <b2dJson.h>

#include "BackgroundLayer.h"
#include "LevelMenuLayer.h"

USING_NS_CC;

LevelLayer::LevelLayer() : ratBody(nullptr), earthBody(nullptr), totalTime(0.0) {}

Scene *LevelLayer::createScene()
{
    auto scene = Scene::create();

    auto backgroundLayer = BackgroundLayer::create("cheese01.png", "background01.png");
    scene->addChild(backgroundLayer);

    auto levelMenuLayer = LevelMenuLayer::create();
    scene->addChild(levelMenuLayer);

    auto layer = LevelLayer::create();
    scene->addChild(layer);

    return scene;
}

bool LevelLayer::init()
{
    if (!RUBELayer::init()) {
        return false;
    }

    return true;
}

void LevelLayer::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform,
                      uint32_t transformUpdated)
{
    RUBELayer::draw(renderer, transform, transformUpdated);

    b2Color redColor(1, 0, 0);
    b2Color blueColor(0, 0, 1);

    b2Vec2 earthCenter = earthBody->GetPosition();
    b2Vec2 ratCenter = ratBody->GetPosition();

    m_debugDraw->DrawSegment(ratCenter, earthCenter, redColor);

    m_debugDraw->DrawSegment(ratCenter, ratCenter + propellerForce, redColor);

    m_debugDraw->DrawSegment(
        ratCenter, ratCenter + ratBody->GetLinearVelocityFromLocalPoint(b2Vec2_zero), blueColor);
}

std::string LevelLayer::getFilename() { return "test.json"; }

Point LevelLayer::initialWorldOffset()
{
    // This function should return the location in pixels to place
    // the (0,0) point of the physics world. The screen position
    // will be relative to the bottom left corner of the screen.

    // place (0,0) of physics world at center of bottom edge of screen
    Size s = Director::getInstance()->getWinSize();
    return Vec2(s.width / 2, 0);
}

float LevelLayer::initialWorldScale()
{
    // This method should return the number of pixels for one physics unit.
    // When creating the scene in RUBE I can see that the jointTypes scene
    // is about 8 units high, so I want the height of the view to be about
    // 10 units, which for iPhone in landscape (480x320) we would return 32.
    // But for an iPad in landscape (1024x768) we would return 76.8, so to
    // handle the general case, we can make the return value depend on the
    // current screen height.

    Size s = Director::getInstance()->getWinSize();
    return s.height / 10; // screen will be 10 physics units high
}

void LevelLayer::afterLoadProcessing(b2dJson *json)
{
    RUBELayer::afterLoadProcessing(json);

    ratBody = json->getBodyByName("rat");
    earthBody = json->getBodyByName("earth");

    assert(ratBody);
    assert(earthBody);
}

void LevelLayer::update(float dt)
{
    totalTime += dt;

    float desiredSpeed = b2Min(3.0f, floorf(totalTime + 1.0));

    RUBELayer::update(dt);

    b2Vec2 earthCenter = earthBody->GetPosition();
    b2Vec2 ratCenter = ratBody->GetPosition();

    propellerForce = (earthCenter - ratCenter).Skew();
    propellerForce.Normalize();

    float v = b2Dot(ratBody->GetLinearVelocity(), propellerForce);

    if (v < desiredSpeed) {
        propellerForce *= -(desiredSpeed - v) / 25;

        // CCLOG("%f %f", v, desiredSpeed - v);

        ratBody->ApplyLinearImpulse(propellerForce, ratBody->GetWorldCenter(), true);
    }
}