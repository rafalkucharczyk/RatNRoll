#include "LevelLayer.h"

#include <b2dJson.h>

USING_NS_CC;

#include <limits>

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
                levelLayer->gameFinishedCallback(levelLayer->score);
            }
        }

        for (LevelLayer::BodiesList::iterator i = levelLayer->itemsBodies.begin();
             i != levelLayer->itemsBodies.end(); i++) {
            if (isContact(contact, *i, levelLayer->ratBody)) {
                b2Body *body = *i;

                levelLayer->ratAteItem(
                    static_cast<DropItemUserData *>(body->GetUserData())->itemType);

                auto scaleAction = ScaleTo::create(0.1, 0.0);
                auto removeAction = CallFunc::create([body, this]() {
                    levelLayer->itemsToRemove.push_back(body);

                });

                auto sequenceAction = Sequence::create(scaleAction, removeAction, nullptr);
                levelLayer->getAnySpriteOnBody(body)->runAction(sequenceAction);

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
    : ratBody(nullptr), earthBody(nullptr), cageBody(nullptr), ratMinSpeed(0.5), ratMaxSpeed(5.0),
      ratSpeedStep(0.5), ratTargetSpeed(ratMinSpeed), score(0),
      previousRevoluteJointAngle(std::numeric_limits<float>::min()), scoreLabel(nullptr),
      totalTime(0.0), contactListener(new LevelContactListener(this))
{
}

bool LevelLayer::init()
{
    if (!RUBELayer::init()) {
        return false;
    }

    m_world->SetContactListener(contactListener.get());

    schedule(schedule_selector(LevelLayer::dropItem), 3.0);

    initScoreLabel();

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

    b2JointEdge *i = earthBody->GetJointList();

    while (i != nullptr) {
        if (i->joint->GetType() == e_revoluteJoint) {
            break;
        }

        i = i->next;
    }

    earthRevoluteJoint = dynamic_cast<b2RevoluteJoint *>(i->joint);
}

void LevelLayer::update(float dt)
{
    RUBELayer::update(dt);
    totalTime += dt;

    removeOutstandingItems();

    doPhysicsCalculationStep();

    updateScore();
}

bool LevelLayer::setCustomImagePositionsFromPhysicsBodies(const RUBEImageInfo *imageInfo,
                                                          cocos2d::Point &position, float &angle)
{
    if (imageInfo->name == "rat") // special handling for rat image
    {
        // adapt position
        b2Vec2 p = imageInfo->body->GetPosition();
        b2Fixture *fixture = imageInfo->body->GetFixtureList();
        assert(fixture && fixture->GetType() == b2Shape::e_circle); // rat model is just a ball
        float radius = fixture->GetShape()->m_radius;
        position = Vec2(p.x, p.y + 2 * radius);

        // orient texture along the radius from rat to earth's center
        b2Vec2 r = p - earthBody->GetPosition();
        r.Normalize();
        b2Vec2 v = b2Vec2(0, 1);

        float d = acos(b2Dot(r, v));

        if (!isnan(d)) {
            angle += d;

            if (p.x < 0) {
                angle = -angle;
            }
        }

        return true;
    }

    return false;
}

void LevelLayer::doPhysicsCalculationStep()
{
    // increase/decrease speed
    ratBody->ApplyAngularImpulse(ratTargetSpeed / 10, true);

    // custom gravity
    b2Vec2 earthCenter = earthBody->GetPosition();
    b2Vec2 ratCenter = ratBody->GetPosition();

    b2Vec2 g = earthCenter - ratCenter;
    g.Normalize();
    g *= 8;
    ratBody->ApplyForce(ratBody->GetMass() * g, ratBody->GetWorldCenter(), true);
}

void LevelLayer::updateScore()
{
    float currentAngle = -earthRevoluteJoint->GetJointAngle();
    if (currentAngle - previousRevoluteJointAngle > 0.2) {
        score++;

        scoreLabel->setString(std::to_string(score));

        previousRevoluteJointAngle = currentAngle;
    } else if (currentAngle < previousRevoluteJointAngle) {
        previousRevoluteJointAngle = currentAngle;
    }
}

void LevelLayer::dropItem(float t)
{
    ItemType itemType = randomizeItemType();

    b2Body *body = jsonParser.j2b2Body(m_world, itemJsons[itemType]);
    body->SetUserData(new DropItemUserData(itemType));

    duplicateImageForBody(itemTypeToImageName(itemType), body);
    body->ApplyAngularImpulse(0.1 * body->GetMass(), true);

    body->SetTransform(b2Vec2(rand_minus1_1() * 1.5, 10), rand_0_1() * 2 * M_PI);

    itemsBodies.push_back(body);

    auto delayAction = DelayTime::create(3);
    auto fadeAction = FadeTo::create(0.5, 0);
    auto removeAction = CallFunc::create([body, this]() {
        auto i = std::find(itemsBodies.begin(), itemsBodies.end(), body);

        if (i != itemsBodies.end()) // might have been already eaten by rat
        {
            itemsBodies.erase(i);

            itemsToRemove.push_back(body);
        }
    });

    auto sequenceAction = Sequence::create(delayAction, fadeAction, removeAction, nullptr);
    getAnySpriteOnBody(body)->runAction(sequenceAction);
}

LevelLayer::ItemType LevelLayer::randomizeItemType() const
{
    ItemType itemType = static_cast<ItemType>(rand() % ITEM_TYPE_MAX);

    if (ratTargetSpeed == ratMaxSpeed) {
        itemType = SLOWDOWN;
    }

    if (ratTargetSpeed == ratMinSpeed) {
        itemType = SPEEDUP;
    }

    return itemType;
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
        ratTargetSpeed = b2Min(ratTargetSpeed + ratSpeedStep, ratMaxSpeed);
        backgroundSpeedFunction(1);
    }

    if (itemType == SLOWDOWN) {
        ratTargetSpeed = b2Max(ratTargetSpeed - ratSpeedStep, ratMinSpeed);
        backgroundSpeedFunction(-1);
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

void LevelLayer::initScoreLabel()
{
    scoreLabel = Label::createWithSystemFont("", "Marker Felt", 60);
    scoreLabel->setColor(Color3B::BLACK);

    addChild(scoreLabel, 1);

    scoreLabel->setScale(1 / getScale());

    b2Vec2 pos = earthBody->GetWorldCenter();
    scoreLabel->setPosition(pos.x, pos.y);
    scoreLabel->setString(std::to_string(score));
}