#include "LevelLayer.h"

#include <b2dJson.h>

USING_NS_CC;

class DropItemUserData
{
  public:
    DropItemUserData(LevelLayer::ItemType type) : itemType(type) {}
    LevelLayer::ItemType itemType;
};

class LevelContactListener : public b2ContactListener
{
  public:
    LevelContactListener(LevelLayer *levelLayer) : levelLayer(levelLayer) {}

    void BeginContact(b2Contact *contact)
    {
        if (isContact(contact, levelLayer->ratBody, levelLayer->cageBody)) {
            if (levelLayer->gameFinishedCallback) {
                levelLayer->gameFinishedCallback();
            }
        }

        for (LevelLayer::BodiesList::const_iterator i = levelLayer->itemsBodies.begin();
             i != levelLayer->itemsBodies.end(); i++) {
            if (isContact(contact, *i, levelLayer->ratBody)) {
                levelLayer->ratAteItem(
                    static_cast<DropItemUserData *>((*i)->GetUserData())->itemType);
            }

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
};

LevelLayer::LevelLayer()
    : ratBody(nullptr), earthBody(nullptr), cageBody(nullptr), ratTargetSpeed(3.0), totalTime(0.0),
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
    assert(ratBody);
    earthBody = json->getBodyByName("earth");
    assert(earthBody);
    cageBody = json->getBodyByName("cage");
    assert(cageBody);

    for (int i = SPEEDUP; i < ITEM_TYPE_MAX; i++) {
        ItemType itemType = static_cast<ItemType>(i);
        b2Body *body = json->getBodyByName(itemTypeToImageName(itemType));
        assert(body);
        itemJsons[itemType] = json->b2j(body);
    }
}

void LevelLayer::update(float dt)
{
    RUBELayer::update(dt);
    totalTime += dt;

    removeOutstandingItems();

    doCalculationStep();
}

void LevelLayer::doCalculationStep()
{
    b2Vec2 earthCenter = earthBody->GetPosition();
    b2Vec2 ratCenter = ratBody->GetPosition();

    propellerForce = (earthCenter - ratCenter).Skew();
    propellerForce.Normalize();

    b2Vec2 v = ratBody->GetLinearVelocity();
    float vv = v.Length();

    if (vv < ratTargetSpeed) {
        propellerForce *= -0.1 * (ratTargetSpeed - vv);

        ratBody->ApplyLinearImpulse(propellerForce, ratBody->GetWorldCenter(), true);
    }
}

void LevelLayer::dropItem(float t)
{
    ItemType itemType = static_cast<ItemType>(rand() % ITEM_TYPE_MAX);

    b2Body *body = jsonParser.j2b2Body(m_world, itemJsons[itemType]);
    body->SetUserData(new DropItemUserData(itemType));

    duplicateImageForBody(itemTypeToImageName(itemType), body);
    body->ApplyAngularImpulse(0.1 * body->GetMass(), true);

    body->SetTransform(b2Vec2(rand_minus1_1() * 1.5, 10), rand_0_1() * 2 * M_PI);

    itemsBodies.push_back(body);
}

void LevelLayer::removeOutstandingItems()
{
    for (auto i = itemsToRemove.begin(); i != itemsToRemove.end(); i++) {
        delete static_cast<DropItemUserData *>((*i)->GetUserData());
        removeBodyFromWorld(*i);
    }

    itemsToRemove.clear();
}

void LevelLayer::ratAteItem(ItemType itemType)
{
    if (itemType == SPEEDUP) {
        ratTargetSpeed = b2Min(ratTargetSpeed + 0.5, 10.0);
    }

    if (itemType == SLOWDOWN) {
        ratTargetSpeed = b2Max(ratTargetSpeed - 0.5, 1.0);
    }
}

std::string LevelLayer::itemTypeToImageName(LevelLayer::ItemType itemType) const
{
    if (itemType == SPEEDUP) {
        return "item_speedup";
    }

    if (itemType == SLOWDOWN) {
        return "item_slowdown";
    }

    assert(false);

    return "";
}