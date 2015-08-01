#include "LevelLayer.h"

#include <limits>
#include <b2dJson.h>

USING_NS_CC;

class DropItemUserData
{
  public:
    DropItemUserData(LevelCustomization::ItemType type) : itemType(type) {}
    LevelCustomization::ItemType itemType;
};

class LevelContactListener : public b2ContactListener
{
  public:
    LevelContactListener(LevelLayer *levelLayer) : levelLayer(levelLayer) {}

    void BeginContact(b2Contact *contact)
    {
        if (isContact(contact, levelLayer->ratBody, levelLayer->cageBody)) {
            if (levelLayer->gameFinishedCallback) {
                levelLayer->gameFinishedCallback(levelLayer->gameScore);
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

class LevelLayerProxyImpl : public LevelLayerProxy
{
  public:
    LevelLayerProxyImpl(LevelLayer &levelLayer, b2Body *body) : levelLayer(levelLayer), body(body)
    {
    }
    void pause() override { levelLayer.pauseLevel(); }
    void resume() override
    {
        levelLayer.resumeLevel();

        if (body) {
            levelLayer.runDropItemAction(body);
        } else {
            levelLayer.startDroppingItems();
        }
    }
    void addOverlayingLayer(Layer *layer)
    {
        Director::getInstance()->getRunningScene()->addChild(layer, 0,
                                                             LevelLayer::proxyOverlayLayer);
    }

  private:
    LevelLayer &levelLayer;
    b2Body *body;
};

const std::string LevelLayer::name = "LevelLayer";

LevelLayer::LevelLayer(LevelCustomization *customization)
    : levelCustomization(customization), ratBody(nullptr), earthBody(nullptr), cageBody(nullptr),
      ratSpeed(levelCustomization->getRatSpeedMin()), applyCustomGravity(true), gameScore(0),
      previousRevoluteJointAngle(std::numeric_limits<float>::min()), scoreLabel(nullptr),
      totalTime(0.0), paused(false), contactListener(new LevelContactListener(this))
{
}

bool LevelLayer::init()
{
    if (!RUBELayer::init()) {
        return false;
    }

    setName(name);

    m_world->SetContactListener(contactListener.get());

    scoreLabel = initScoreLabel(gameScore);

    runCustomActionOnStart();

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

        m_debugDraw->DrawSegment(ratCenter,
                                 ratCenter + ratBody->GetLinearVelocityFromLocalPoint(b2Vec2_zero),
                                 blueColor);
    }
}

LevelLayer *LevelLayer::create(LevelCustomization *customization)
{
    LevelLayer *ret = new (std::nothrow) LevelLayer(customization);
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    } else {
        delete ret;
        ret = nullptr;
        return nullptr;
    }
}

std::string LevelLayer::getFilename() { return levelCustomization->getRubeJsonFileName(); }

Point LevelLayer::initialWorldOffset()
{
    // This function should return the location in pixels to place
    // the (0,0) point of the physics world. The screen position
    // will be relative to the bottom left corner of the screen.

    // place (0,0) of physics world at center of bottom edge of screen
    Size s = Director::getInstance()->getWinSize();
    return Vec2(s.width / 2, s.height / 20);
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

    for (int i = LevelCustomization::SPEEDUP; i < LevelCustomization::ITEM_TYPE_MAX; i++) {
        LevelCustomization::ItemType itemType = static_cast<LevelCustomization::ItemType>(i);
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
    if (paused) {
        return;
    }

    RUBELayer::update(dt);
    totalTime += dt;

    removeOutstandingItems();

    doPhysicsCalculationStep();

    updateScore();
}

bool LevelLayer::setCustomImagePositionsFromPhysicsBodies(const RUBEImageInfo *imageInfo,
                                                          cocos2d::Point &position, float &angle)
{
    if (imageInfo->body == ratBody) // special handling for rat image
    {
        b2Vec2 p = imageInfo->body->GetPosition();

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

        // adapt position
        b2Fixture *fixture = imageInfo->body->GetFixtureList();
        assert(fixture && fixture->GetType() == b2Shape::e_circle); // rat model is just a ball
        float radius = fixture->GetShape()->m_radius;

        position = Vec2(p.x, p.y) - Vec2(sin(angle) * radius, cos(angle) * radius);

        return true;
    }

    return false;
}

bool LevelLayer::isFixtureTouchable(b2Fixture *fixture) { return fixture->GetBody() == earthBody; }

void LevelLayer::runCustomActionOnStart()
{
    scheduleOnce([this](float t) {
        std::shared_ptr<LevelLayerProxyImpl> levelLayerProxy(
            new LevelLayerProxyImpl(*this, nullptr));
        FiniteTimeAction *customAction = levelCustomization->levelStartedCallback(levelLayerProxy);

        if (customAction) {
            runAction(customAction);
        } else {
            startDroppingItems();
        }
    }, 0.5, "dummy");
}

void LevelLayer::startDroppingItems()
{
    schedule(schedule_selector(LevelLayer::dropItem), levelCustomization->getItemDropInterval());
}

void LevelLayer::stopDroppingItems() { unschedule(schedule_selector(LevelLayer::dropItem)); }

void LevelLayer::doPhysicsCalculationStep()
{
    b2Vec2 earthCenter = earthBody->GetPosition();
    b2Vec2 ratCenter = ratBody->GetPosition();

    b2Fixture *earthFixture = earthBody->GetFixtureList();
    // earth fixture is a circle
    assert(earthFixture && earthFixture->GetType() == b2Shape::e_circle);
    float radius = earthFixture->GetShape()->m_radius;

    // apply speed, more when running uphill (b), less when running downhill (a)
    float x = (ratCenter.x - earthCenter.x) / (2 * radius);
    Vec2 a(-0.5, 0.04), b(0.5, 0.2);

    float speedRatio = ((b.y - a.y) / (b.x - a.x)) * x + (b.x * a.y - a.x * b.y) / (b.x - a.x);
    ratBody->ApplyAngularImpulse(ratSpeed * speedRatio, true);

    // custom gravity, stop applying at the bottom of earth
    if (ratCenter.y > earthCenter.y - radius * 0.33 && applyCustomGravity) {
        b2Vec2 g = earthCenter - ratCenter;
        g.Normalize();
        g *= 30;
        ratBody->ApplyForce(ratBody->GetMass() * g, ratBody->GetWorldCenter(), true);
    }
}

void LevelLayer::updateScore()
{
    float currentAngle = -earthRevoluteJoint->GetJointAngle();
    if (currentAngle - previousRevoluteJointAngle > 0.2) {
        gameScore++;

        scoreLabel->setString(std::to_string(gameScore));

        previousRevoluteJointAngle = currentAngle;
    } else if (currentAngle < previousRevoluteJointAngle) {
        previousRevoluteJointAngle = currentAngle;
    }
}

void LevelLayer::dropItem(float t)
{
    LevelCustomization::ItemType itemType = levelCustomization->getDropItemType(ratSpeed);

    if (itemType == LevelCustomization::ITEM_TYPE_MAX) {
        return;
    }

    b2Body *body = duplicateItem(itemType);

    runShowItemAction(body);

    itemsBodies.push_back(body);

    std::shared_ptr<LevelLayerProxyImpl> levelLayerProxy(new LevelLayerProxyImpl(*this, body));
    FiniteTimeAction *customAction =
        levelCustomization->itemAddedCallback(levelLayerProxy, itemType);

    if (customAction) {
        runAction(customAction);
    } else {
        runDropItemAction(body);
    }
}

b2Body *LevelLayer::duplicateItem(LevelCustomization::ItemType itemType)
{
    b2Body *body = jsonParser.j2b2Body(m_world, itemJsons[itemType]);
    body->SetUserData(new DropItemUserData(itemType));
    body->SetTransform(levelCustomization->getDropItemSpot(), rand_0_1() * 2 * M_PI);

    duplicateImageForBody(itemTypeToImageName(itemType), body);
    body->SetGravityScale(0.0);
    body->ApplyAngularImpulse(0.1 * body->GetMass(), true);

    return body;
}

void LevelLayer::runShowItemAction(b2Body *body)
{
    float targetScale = getAnySpriteOnBody(body)->getScale();
    getAnySpriteOnBody(body)->setScale(0);
    getAnySpriteOnBody(body)->setVisible(true);
    getAnySpriteOnBody(body)->runAction(EaseBackOut::create(ScaleTo::create(0.2, targetScale)));
}

void LevelLayer::runDropItemAction(b2Body *body)
{
    auto dropDelayAction = DelayTime::create(0.5);
    auto applyGravityAction = CallFunc::create([body]() { body->SetGravityScale(1.0); });
    auto removeDelayAction = DelayTime::create(3);
    auto fadeAction = FadeTo::create(0.5, 0);
    auto removeAction = CallFunc::create([body, this]() {
        auto i = std::find(itemsBodies.begin(), itemsBodies.end(), body);

        if (i != itemsBodies.end()) // might have been already eaten by rat
        {
            itemsBodies.erase(i);

            itemsToRemove.push_back(body);
        }
    });

    auto sequenceAction = Sequence::create(dropDelayAction, applyGravityAction, removeDelayAction,
                                           fadeAction, removeAction, nullptr);
    getAnySpriteOnBody(body)->runAction(sequenceAction);
}

void LevelLayer::removeOutstandingItems()
{
    for (auto i = itemsToRemove.begin(); i != itemsToRemove.end(); i++) {
        DropItemUserData *itemData = static_cast<DropItemUserData *>((*i)->GetUserData());
        levelCustomization->itemRemovedCallback(itemData->itemType);
        delete itemData;

        removeBodyFromWorld(*i);
    }

    itemsToRemove.clear();
}

void LevelLayer::ratAteItem(LevelCustomization::ItemType itemType)
{
    if (itemType == LevelCustomization::SPEEDUP) {
        speedUpItemEaten();
    }

    if (itemType == LevelCustomization::SLOWDOWN) {
        slowDownItemEaten();
    }

    if (itemType == LevelCustomization::HOVER) {
        hoverItemEaten();
    }

    if (itemType == LevelCustomization::HALVE) {
        halveItemEaten();
    }
}

void LevelLayer::speedUpItemEaten()
{
    ratSpeed = b2Min(ratSpeed + levelCustomization->getRatSpeedStep(),
                     levelCustomization->getRatSpeedMax());
    backgroundSpeedFunction(1);

    getRatAnimation()->setTimeScale(getRatAnimation()->getTimeScale() * 1.2);
}

void LevelLayer::slowDownItemEaten()
{
    ratSpeed = b2Max(ratSpeed - levelCustomization->getRatSpeedStep(),
                     levelCustomization->getRatSpeedMin());
    backgroundSpeedFunction(-1);

    getRatAnimation()->setTimeScale(getRatAnimation()->getTimeScale() / 1.2);
}

void LevelLayer::hoverItemEaten()
{
    applyCustomGravity = false;
    ratBody->SetGravityScale(-2);

    float lastRatSpeed = ratSpeed;
    ratSpeed = 0;
    stopDroppingItems();

    float lastTimeScale = getRatAnimation()->getTimeScale();
    getRatAnimation()->setTimeScale(0);

    getAnySpriteOnBody(ratBody)->runAction(Sequence::create(
        DelayTime::create(2.0), CallFunc::create([this, lastRatSpeed, lastTimeScale]() {
            startDroppingItems();
            applyCustomGravity = true;
            ratBody->SetGravityScale(1.0);
            ratSpeed = lastRatSpeed;
            this->getRatAnimation()->setTimeScale(lastTimeScale);
        }),
        nullptr));
}

void LevelLayer::halveItemEaten()
{
    int halfScore = gameScore / 2;
    gameScore -= halfScore;

    scoreLabel->setString(std::to_string(gameScore));

    // show animation with number of points taken
    auto label = initScoreLabel(halfScore);
    float animationDuration = 0.5;

    label->runAction(Sequence::create(
        Spawn::create(ScaleTo::create(animationDuration, 0),
                      MoveBy::create(animationDuration,
                                     Vec2(0, -label->getContentSize().height / getScale())),
                      nullptr),
        RemoveSelf::create(), nullptr));
}

std::string LevelLayer::itemTypeToImageName(LevelCustomization::ItemType itemType) const
{
    if (itemType == LevelCustomization::SPEEDUP) {
        return "item_speedup";
    }

    if (itemType == LevelCustomization::SLOWDOWN) {
        return "item_slowdown";
    }

    if (itemType == LevelCustomization::HOVER) {
        return "item_hover";
    }

    if (itemType == LevelCustomization::HALVE) {
        return "item_halve";
    }

    assert(false);

    return "";
}

Label *LevelLayer::initScoreLabel(int score)
{
    auto label = Label::createWithSystemFont("", "Marker Felt", 60);
    label->setColor(Color3B::BLACK);

    addChild(label, 1);

    label->setScale(1 / getScale());

    b2Vec2 pos = earthBody->GetWorldCenter();
    label->setPosition(pos.x, pos.y);
    label->setString(std::to_string(score));

    return label;
}

spine::SkeletonAnimation *LevelLayer::getRatAnimation()
{
    spine::SkeletonAnimation *skeletonAnimation =
        dynamic_cast<spine::SkeletonAnimation *>(getAnySpriteOnBody(ratBody));
    assert(skeletonAnimation);

    return skeletonAnimation;
}
