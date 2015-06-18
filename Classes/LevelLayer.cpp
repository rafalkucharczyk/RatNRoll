#include "LevelLayer.h"

#include <b2dJson.h>

USING_NS_CC;

class LevelContactListener : public b2ContactListener
{
  public:
    LevelContactListener(LevelLayer *levelLayer)
        : levelLayer(levelLayer), gameFinishedNotified(false)
    {
    }

    void BeginContact(b2Contact *contact)
    {
        if (isContact(contact, levelLayer->ratBody, levelLayer->cageBody)) {
            if (!gameFinishedNotified) {
                if (levelLayer->gameFinishedCallback) {
                    levelLayer->gameFinishedCallback();
                }

                gameFinishedNotified = true;
            }
        }

        for (LevelLayer::BodiesList::const_iterator i = levelLayer->itemsBodies.begin();
             i != levelLayer->itemsBodies.end(); i++) {
            if (isContact(contact, *i, levelLayer->cageBody) ||
                isContact(contact, *i, levelLayer->ratBody)) {
                levelLayer->itemsToRemove.push_back(*i);
                i = levelLayer->itemsBodies.erase(i);
            }
        }
    }

    void EndContact(b2Contact *contact) {}

  private:
    // helper function to figure out if there is a contact between two bodies
    bool isContact(b2Contact *contact, b2Body *firstBody, b2Body *secondBody)
    {
        b2Body *bodyA = contact->GetFixtureA()->GetBody();
        b2Body *bodyB = contact->GetFixtureB()->GetBody();

        return ((bodyA == firstBody && bodyB == secondBody) ||
                (bodyB == firstBody && bodyA == secondBody));
    }

  private:
    LevelLayer *levelLayer;
    bool gameFinishedNotified;
};

LevelLayer::LevelLayer()
    : ratBody(nullptr), earthBody(nullptr), totalTime(0.0),
      contactListener(new LevelContactListener(this))
{
}

bool LevelLayer::init()
{
    if (!RUBELayer::init()) {
        return false;
    }

    m_world->SetContactListener(contactListener.get());

    schedule(schedule_selector(LevelLayer::dropItem), 3.0);

    return true;
}

void LevelLayer::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform,
                      uint32_t transformUpdated)
{
    RUBELayer::draw(renderer, transform, transformUpdated);

    if (drawDebugData()) {
        b2Color redColor(1, 0, 0);
        b2Color blueColor(0, 0, 1);

        b2Vec2 earthCenter = earthBody->GetPosition();
        b2Vec2 ratCenter = ratBody->GetPosition();

        m_debugDraw->DrawSegment(ratCenter, earthCenter, redColor);

        m_debugDraw->DrawSegment(ratCenter, ratCenter + propellerForce, redColor);

        m_debugDraw->DrawSegment(ratCenter,
                                 ratCenter + ratBody->GetLinearVelocityFromLocalPoint(b2Vec2_zero),
                                 blueColor);
    }
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
    cageBody = json->getBodyByName("cage");

    b2Body *itemSpeedup = json->getBodyByName("item_speedup");
    assert(itemSpeedup);
    itemSpeedupJson = json->b2j(itemSpeedup);

    assert(ratBody);
    assert(earthBody);
    assert(cageBody);
}

void LevelLayer::update(float dt)
{
    RUBELayer::update(dt);

    removeOutstandingItems();

    totalTime += dt;

    float desiredSpeed = b2Min(3.0f, floorf(totalTime + 1.0));

    b2Vec2 earthCenter = earthBody->GetPosition();
    b2Vec2 ratCenter = ratBody->GetPosition();

    propellerForce = (earthCenter - ratCenter).Skew();
    propellerForce.Normalize();

    b2Vec2 v = ratBody->GetLinearVelocity();
    float vv = v.Length();

    if (vv < desiredSpeed) {
        propellerForce *= -0.1 * (desiredSpeed - vv);

        ratBody->ApplyLinearImpulse(propellerForce, ratBody->GetWorldCenter(), true);
    }
}

void LevelLayer::dropItem(float t)
{
    b2Body *body = jsonParser.j2b2Body(m_world, itemSpeedupJson);
    duplicateImageForBody("item_speedup", body);
    body->ApplyAngularImpulse(0.1 * body->GetMass(), true);

    body->SetTransform(b2Vec2(rand_minus1_1() * 1.5, 10), rand_0_1() * 2 * M_PI);

    itemsBodies.push_back(body);
}

void LevelLayer::removeOutstandingItems()
{
    for (BodiesList::const_iterator i = itemsToRemove.begin(); i != itemsToRemove.end(); i++) {
        removeBodyFromWorld(*i);
    }

    itemsToRemove.clear();
}