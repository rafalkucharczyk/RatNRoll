#include "LevelLayer.h"
#include "ToString.h"

#include <iomanip>

#include <limits>
#include <cmath>
#include <b2dJson.h>

#include "SoundHelper.h"
#include "AchievementTracker.h"

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
            levelLayer->handleGameOver();
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
    LevelLayerProxyImpl(LevelLayer &levelLayer, b2Body *body)
        : levelLayer(levelLayer), body(body), ratAnimationTimeScale(1.0)
    {
    }
    void pause() override
    {
        levelLayer.pauseLevel();
        ratAnimationTimeScale = levelLayer.getRatAnimation()->getTimeScale();
        levelLayer.getRatAnimation()->setTimeScale(0);
    }
    void resume() override
    {
        levelLayer.resumeLevel();
        levelLayer.getRatAnimation()->setTimeScale(ratAnimationTimeScale);

        if (body) {
            levelLayer.runDropItemAction(body);
        } else {
            levelLayer.scheduleOnce([this](float) { levelLayer.startDroppingItems(); }, 3.0,
                                    "LevelLayerProxyImpl_resume");
        }
    }
    void quit() override
    {
        if (levelLayer.gameCompletedCallback) {
            levelLayer.gameCompletedCallback();
        }
    }
    void addOverlayingLayer(Layer *layer) override
    {
        Director::getInstance()->getRunningScene()->addChild(layer, 0,
                                                             LevelLayer::proxyOverlayLayerTag);
    }

  private:
    LevelLayer &levelLayer;
    b2Body *body;
    float ratAnimationTimeScale;
};

class ShadowRatHelper
{
  public:
    ShadowRatHelper(LevelLayer &levelLayer, Json::Value shadowRatJson)
        : levelLayer(levelLayer), shadowRatJson(shadowRatJson)
    {
    }

  private:
    struct ShadowRatEntry {
        ShadowRatEntry() : scoreTo(0), scoreFromNumber(-1), body(nullptr) {}
        ShadowRatEntry(std::function<int(int)> scoreFromFunc, int scoreTo)
            : scoreFromFunc(scoreFromFunc), scoreTo(scoreTo), scoreFromNumber(-1), body(nullptr)
        {
        }

        void reset()
        {
            body = nullptr;
            scoreFromNumber = -1;
        }

        std::function<int(int)> scoreFromFunc;
        int scoreFromNumber;
        int scoreTo;
        b2Body *body;
    };

  public:
    void addShadow(const std::string &playerName, std::function<int(int)> scoreFrom, int scoreTo)
    {
        if (shadowEntries.find(playerName) == shadowEntries.end()) {
            shadowEntries[playerName] = ShadowRatEntry(scoreFrom, scoreTo);
        }
    }

    void doPhysicsCalculationStep()
    {
        for (auto i = shadowEntries.begin(); i != shadowEntries.end(); i++) {
            if (i->second.body == nullptr) {
                continue;
            }

            b2Vec2 vv = levelLayer.earthBody->GetPosition() +
                        b2Vec2(0, levelLayer.getEarthRadius()) - i->second.body->GetPosition();
            vv.Normalize();

            double f = 30;
            // apply much more "gravity" when shadow rat is falling down, so it does not bounce
            if ((i->second.body->GetPosition() - levelLayer.earthBody->GetPosition()).Length() >
                1.1 * levelLayer.getEarthRadius()) {
                f = 300;
            }

            i->second.body->ApplyForceToCenter((f + 30 * rand_0_1()) * vv, true);
        }
    }

    void scoreUpdated(int newScore)
    {
        // newScore is worse: -1, still in range: 0, better: 1
        auto inRange = [newScore, this](ShadowRatEntry &entry) -> int {
            int scoreFrom = entry.scoreFromFunc(levelLayer.getGameScoreDelta());

            if (entry.scoreFromNumber != -1) {
                scoreFrom = entry.scoreFromNumber;
            }

            // once there is a body visible, remember scoreFrom value obtained from function
            // so the threshold do not depend on speed anymore
            if (entry.body && entry.scoreFromNumber == -1) {
                entry.scoreFromNumber = scoreFrom;
            }

            if (newScore < scoreFrom) {
                return -1;
            }

            if (newScore > entry.scoreTo) {
                return 1;
            }

            return 0;
        };

        for (auto i = shadowEntries.begin(); i != shadowEntries.end(); i++) {
            int res = inRange(i->second);

            if (res != 0 && i->second.body != nullptr) {
                if (levelLayer.shadowRatActionCallback) {
                    levelLayer.shadowRatActionCallback(i->first, i->second.scoreTo, res);
                }

                removeShadowRat(i->second.body);
                i->second.reset();
            }
        }

        // only one shadow rat allowed, return if we still have one
        if (std::find_if(shadowEntries.begin(), shadowEntries.end(),
                         [](ShadowEntries::value_type &v) { return v.second.body != nullptr; }) !=
            shadowEntries.end()) {
            return;
        }

        for (auto i = shadowEntries.begin(); i != shadowEntries.end(); i++) {
            if (inRange(i->second) == 0 && i->second.body == nullptr) {
                if (levelLayer.shadowRatActionCallback) {
                    levelLayer.shadowRatActionCallback(i->first, i->second.scoreTo, 0);
                }

                i->second.body = insertShadowRat(i->first, i->second.scoreTo);
                assert(i->second.body);
                return; // only one shadow rat allowed
            };
        }
    }

  private:
    b2Body *insertShadowRat(const std::string &playerName, int score)
    {
        b2Body *body = levelLayer.jsonParser.j2b2Body(levelLayer.m_world, shadowRatJson);
        body->SetTransform(b2Vec2(0, 10), 0);

        levelLayer.duplicateImageForBody("rat_shadow", body);

        levelLayer.getAnySpriteOnBody(body)->setVisible(true);
        Node *node = levelLayer.getAnySpriteOnBody(body);

        addPlayerNameLabelToNode(node, playerName, score);

        return body;
    }

    void removeShadowRat(b2Body *body)
    {
        levelLayer.getAnySpriteOnBody(body)->runAction(Sequence::create(
            CallFunc::create([=]() { body->SetGravityScale(-10); }), DelayTime::create(0.5),
            CallFunc::create([=]() { levelLayer.removeBodyFromWorld(body); }), nullptr));
    }

    void addPlayerNameLabelToNode(Node *node, const std::string &playerName, int /* score */)
    {
        auto label =
            Label::createWithTTF("", "fonts/rat.ttf", 80 * MenuHelper::getContentScaleFactor());
        label->setColor(Color3B::BLACK);
        label->runAction(Sequence::create(
            DelayTime::create(1.5), Spawn::create(ScaleTo::create(0.5, 0.75),
                                                  FadeTo::create(0.5, node->getOpacity()), nullptr),
            nullptr));

        Size cs = node->getContentSize();
        label->setPosition(Vec2(cs.width / 2, 1.15 * cs.height));

        label->setAlignment(TextHAlignment::CENTER);
        label->setString(playerName);

        node->addChild(label);
    }

  private:
    LevelLayer &levelLayer;
    Json::Value shadowRatJson; // keeps representation of parsed shadow rat, needed for duplication

    typedef std::map<std::string, ShadowRatEntry> ShadowEntries;
    ShadowEntries shadowEntries;
};

class AnimationHelper
{
  public:
    AnimationHelper(const LevelCustomization &levelCustomization)
        : levelCustomization(levelCustomization), eyesAnimationInProgress(false),
          lastRunningAnimationTimeScale(0), skeleton(nullptr)
    {

        float speedRange =
            levelCustomization.getRatSpeedMax() - levelCustomization.getRatSpeedMin();
        float speedStep = levelCustomization.getRatSpeedStep();

        int numOfSpeedSteps = 1 + speedRange / speedStep;

        std::vector<std::string> availableAnimations = {"run01.x", "run02.x", "run03.x",
                                                        "run04.x", "run05.x", "run06.x"};

        // always start with the slowest animation
        scheduledAnimations.push_back(std::make_pair(availableAnimations.front(), 1.0));

        // skip run01.x and run06.x, added manually
        for (int i = 1; i < availableAnimations.size() - 1; i++) {
            // distribute rest of animations evenly
            int n = ((i % 2) + numOfSpeedSteps - 2) / (availableAnimations.size() - 2);

            for (int j = 0; j < n; j++) {
                scheduledAnimations.push_back(std::make_pair(availableAnimations[i], 1 + 0.5 * j));
            }
        }

        // always end with the fastest animation
        for (int n = 0; n < numOfSpeedSteps - (scheduledAnimations.size() - 1); n++) {
            scheduledAnimations.push_back(
                std::make_pair(availableAnimations.back(), 1.0 + n * 0.5));
        }

        assert(numOfSpeedSteps == scheduledAnimations.size());
        assert(scheduledAnimations.back().first == availableAnimations.back());
    }

    void setSkeleton(spine::SkeletonAnimation *s) { skeleton = s; }

    void playRunningAnimation(float ratSpeed)
    {
        std::string animationName;
        float animationSpeed;

        std::tie(animationName, animationSpeed) = getRunningAnimationParams(ratSpeed);

        changeRunningAnimation(animationName, animationSpeed);
    }

    void playHoveringAnimation() { changeRunningAnimation("hover_loop_01.x", 1.0); }

    /*
     eyes01.x : pojedyncze mrugnięcie (losowe urozmaicenie ?)
     eyes02.x : smutne oczy (po zjedzeniu czachy?)
     eyes03.x : oczy latające w kólko (po zjedzeniiu fiolki?)
     eyes04.x : oczy rozglądające się lewo/prawo (losowe urozmaicenie?)
     eyes05.x : mrużenie oczu (po zjedzeniu fiolki?)
     eyes06.x : zez (losowe urozmaicenie?)
     eyes07.x : zasypianie (zjedzenie hamburgera, losowe urozmaicenie przy wolnej prędkości?)
     eyes08.x : puszcza oczko (fiolka, losowe urozmaicenie?)
     eyes09.x : wielkie zrenice i mruganie, zdziwienie (fiolka, czacha?)
     eyes10.x : podniesinie dolnych powiek, skupienie, lekki wkurw (czacha, fiolka, losowe
     urozmaicenie)
     eyes11.x : opuszczenie gornych powiek, irytacja, konsternacja (czacha, hamburger, losowe
     urozmaicenie)
     */
    enum Eyes {
        EYES_FIRST = 1,
        SINGLE_BLINK = EYES_FIRST,
        SAD,
        DAZED,
        SHIFTY,
        SQUINTED,
        CROSSEYED,
        SLEEPY,
        WINKING,
        SURPRISED,
        FOCUSED,
        IRRITATED,
        EYES_MAX = IRRITATED
    };

    void playEyesAnimation(Eyes eyesMode)
    {
        assert(skeleton);

        if (eyesAnimationInProgress) {
            return;
        }

        std::ostringstream os;
        os << "eyes" << std::setfill('0') << std::setw(2) << eyesMode << ".x";

        skeleton->setStartListener([this](int trackIndex) {
            if (trackIndex == eyesMovingTrackIndex)
                this->eyesAnimationInProgress = true;
        });
        skeleton->setEndListener([this](int trackIndex) {
            if (trackIndex == eyesMovingTrackIndex)
                this->eyesAnimationInProgress = false;
        });

        skeleton->setAnimation(eyesMovingTrackIndex, os.str(), false);
    }

    void playShowShieldAnimation()
    {
        skeleton->setAnimation(shieldShowingTrackIndex, "helmet_in_01.x", false);
    }

    void playHideShieldAnimation()
    {
        skeleton->setAnimation(shieldShowingTrackIndex, "helmet_out_01.x", false);
    }

    void pauseRunningAnimation()
    {
        assert(skeleton);

        spTrackEntry *entry = skeleton->getCurrent(runningTrackIndex);

        if (entry != nullptr) {
            lastRunningAnimationTimeScale = entry->timeScale;
            entry->timeScale = 0;
        }
    }

    void resumeRunningAnimation()
    {
        assert(skeleton);

        spTrackEntry *entry = skeleton->getCurrent(runningTrackIndex);

        if (entry != nullptr && lastRunningAnimationTimeScale > 0) {
            entry->timeScale = lastRunningAnimationTimeScale;
            lastRunningAnimationTimeScale = 0;
        }
    }

  private:
    void changeRunningAnimation(const std::string &newAnimationName, float newAnimationSpeed)
    {
        assert(skeleton);

        if (skeleton->getCurrent(runningTrackIndex) == nullptr) {
            spTrackEntry *entry = skeleton->setAnimation(runningTrackIndex, newAnimationName, true);
            entry->timeScale = newAnimationSpeed;
        } else {
            setMix(newAnimationName);

            skeleton->runAction(CallFunc::create([=]() {
                skeleton->setCompleteListener([=](int trackIndex, int) {
                    if (trackIndex == runningTrackIndex) {
                        spTrackEntry *entry =
                            skeleton->setAnimation(runningTrackIndex, newAnimationName, true);
                        entry->timeScale = newAnimationSpeed;
                    };
                });
            }));
        }
    }

    std::pair<std::string, float> getRunningAnimationParams(float ratSpeed) const
    {
        float speedStage =
            (ratSpeed - levelCustomization.getRatSpeedMin()) / levelCustomization.getRatSpeedStep();
        int animationIndex = std::min(scheduledAnimations.size() - 1,
                                      static_cast<size_t>(std::floor(speedStage + 0.5)));

        return scheduledAnimations[animationIndex];
    }

    void setMix(const std::string &nextAnimation)
    {
        assert(skeleton);

        std::pair<std::string, std::string> mix(
            skeleton->getCurrent(runningTrackIndex)->animation->name, nextAnimation);

        if (mix.first != mix.second && mixesSet.find(mix) == mixesSet.end()) {
            skeleton->setMix(mix.first, mix.second, 0.25);
            mixesSet.insert(mix);
        }
    }

  private:
    const LevelCustomization &levelCustomization;

    spine::SkeletonAnimation *skeleton;

    std::vector<std::pair<std::string, float>> scheduledAnimations;

    std::set<std::pair<std::string, std::string>> mixesSet;

    bool eyesAnimationInProgress;

    float lastRunningAnimationTimeScale;

    static const int runningTrackIndex = 0;
    static const int eyesMovingTrackIndex = 1;
    static const int shieldShowingTrackIndex = 2;
};

const std::string LevelLayer::name = "LevelLayer";

LevelLayer::LevelLayer(LevelCustomization *customization, AchievementTracker &achievementTracker,
                       int initialGameScore, const std::function<void(int)> &bgSpeedFunction)
    : levelCustomization(customization), ratBody(nullptr), earthBody(nullptr), cageBody(nullptr),

      ratSpeed(levelCustomization->getRatSpeedInitial(initialGameScore)), isHovering(false),
      proRunnerInProgress(false), backgroundSpeedFunction(bgSpeedFunction),
      gameScore(initialGameScore), previousRevoluteJointAngle(std::numeric_limits<float>::min()),
      scoreLabel(nullptr), totalTime(0.0), paused(false), nextItemDropTime(0.0),
      contactListener(new LevelContactListener(this)), cheeseFrenzyParticleNode(nullptr),
      halvePointsParticleNode(nullptr), frenzyGameScoreMultiplier(1), skullShieldCount(0),
      animationHelper(new AnimationHelper(*customization)), achievementTracker(achievementTracker)
{
}

bool LevelLayer::init()
{
    if (!RUBELayer::init()) {
        return false;
    }

    setName(name);

    m_world->SetContactListener(contactListener.get());

    scheduleRatEyesAnimations();

    initScoreLabel(gameScore);

    adaptGameSpeed();

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

LevelLayer *LevelLayer::create(LevelCustomization *customization,
                               AchievementTracker &achievementTracker, int initialGameScore,
                               const std::function<void(int)> &bgSpeedFunction)
{
    LevelLayer *ret = new (std::nothrow)
        LevelLayer(customization, achievementTracker, initialGameScore, bgSpeedFunction);
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    } else {
        delete ret;
        ret = nullptr;
        return nullptr;
    }
}

std::list<std::string> LevelLayer::getFilenames()
{
    return {"level_base.json", levelCustomization->getRubeJsonFileName()};
}

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

        /* Apply level-specific adjustmenets of item properties */
        auto props = levelCustomization->getItemProps(itemType);
        for (auto f = body->GetFixtureList(); f; f = f->GetNext()) {
            f->SetRestitution(f->GetRestitution() + props.restitution);
            f->SetFriction(f->GetFriction() + props.friction);
            f->SetDensity(f->GetDensity() + props.density);
        }

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

    b2Body *ratShadowBody = json->getBodyByName("rat_shadow");
    assert(ratShadowBody);
    shadowRatHelper.reset(new ShadowRatHelper(*this, json->b2j(ratShadowBody)));

    animationHelper->setSkeleton(getRatAnimation());

    levelCustomization->additionalAfterLoadProcessing(json);
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

    calculateScore();
}

void LevelLayer::onExit()
{
    RUBELayer::onExit();
    removeOutstandingItems();

    for (auto i = itemsBodies.begin(); i != itemsBodies.end(); i++) {
        DropItemUserData *itemData = static_cast<DropItemUserData *>((*i)->GetUserData());
        delete itemData;
    }
}

void LevelLayer::pauseLevel()
{
    paused = true;
    animationHelper->pauseRunningAnimation();
}

void LevelLayer::resumeLevel()
{
    paused = false;
    animationHelper->resumeRunningAnimation();
}

bool LevelLayer::setCustomImagePositionsFromPhysicsBodies(const RUBEImageInfo *imageInfo,
                                                          cocos2d::Point &position, float &angle)
{
    if (ratBody == nullptr) {
        return false;
    }

    if (imageInfo->name == "rat" || imageInfo->name == "rat_shadow") {
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

        if (imageInfo->name == "rat") {
            position = Vec2(p.x, p.y) - Vec2(sin(angle) * radius, cos(angle) * radius);
        } else {
            b2Vec2 localPos(position.x, position.y);
            b2Rot rot(-angle);
            localPos = b2Mul(rot, localPos) + imageInfo->body->GetPosition();
            position = Vec2(localPos.x, localPos.y);
        }

        return true;
    }

    return false;
}

bool LevelLayer::isFixtureTouchable(b2Fixture *fixture) { return fixture->GetBody() == earthBody; }

void LevelLayer::addShadowRat(const std::string &name, std::function<int(int)> fromScore,
                              int toScore)
{
    if (!name.empty()) {
        shadowRatHelper->addShadow(name, fromScore, toScore);
    }

    scheduleUpdateScore();
}

std::pair<int, int> LevelLayer::getFixedScoreThresholdForGameScore(int gameScore)
{
    return levelCustomization->getFixedScoreThresholdForGameScore(gameScore);
}

BackgroundConfig LevelLayer::getBackgroundConfig() const
{
    return levelCustomization->getBackgroundConfig();
}

void LevelLayer::playBackgroundMusic(float crossFadeDuration) const
{
    SoundHelper::getInstance().playBackgroundMusicCrossfade(
        levelCustomization->getBackgroundMusicAsset(), crossFadeDuration);
}

int LevelLayer::getRatSpeedLevel() const
{
    float speed = 0.5 + ((ratSpeed - levelCustomization->getRatSpeedMin()) /
                         levelCustomization->getRatSpeedStep());
    return static_cast<int>(speed);
}

void LevelLayer::runCustomActionOnStart()
{
    std::shared_ptr<LevelLayerProxyImpl> levelLayerProxy(new LevelLayerProxyImpl(*this, nullptr));
    FiniteTimeAction *customAction =
        levelCustomization->levelStartedCallback(levelLayerProxy, achievementTracker);

    if (customAction) {
        scheduleOnce([this, customAction](float t) { runAction(customAction); }, 0.2,
                     "customStartAction");
    } else {
        scheduleOnce([this](float t) { startDroppingItems(); }, 0.5, "customStartAction");
    }
}

void LevelLayer::startDroppingItems()
{
    nextItemDropTime = totalTime + levelCustomization->getItemDropInterval(gameScore);
    schedule(schedule_selector(LevelLayer::dropItem), 0.1);
}

void LevelLayer::stopDroppingItems() { unschedule(schedule_selector(LevelLayer::dropItem)); }

void LevelLayer::scheduleRatEyesAnimations()
{
    schedule([this](float t) {
        int min = AnimationHelper::EYES_FIRST, max = AnimationHelper::EYES_MAX;
        animationHelper->playEyesAnimation(
            static_cast<AnimationHelper::Eyes>(random(min, max)));
    }, 5.0, "ratEyes");
}

void LevelLayer::doPhysicsCalculationStep()
{
    b2Vec2 earthCenter = earthBody->GetPosition();
    b2Vec2 ratCenter = ratBody->GetPosition();

    float radius = getEarthRadius();

    // apply speed, more when running uphill (b), less when running downhill (a)
    float x = (ratCenter.x - earthCenter.x) / (2 * radius);
    Vec2 a(-0.5, 0.04), b(0.5, 0.2);

    float speedRatio = ((b.y - a.y) / (b.x - a.x)) * x + (b.x * a.y - a.x * b.y) / (b.x - a.x);

    if (ratBody->GetGravityScale() > 0) { // don't apply angular impulse when hovering
        ratBody->ApplyAngularImpulse(ratSpeed * speedRatio, true);
    }

    // custom gravity, stop applying at the bottom of earth
    if (ratCenter.y > earthCenter.y - radius * 0.33 && !isHovering) {
        b2Vec2 g = earthCenter - ratCenter;
        g.Normalize();
        g *= 30;
        ratBody->ApplyForce(ratBody->GetMass() * g, ratBody->GetWorldCenter(), true);

        // handle pro runner achivement
        // 1. get below center of earth with max speed...
        if ((ratCenter.y < earthCenter.y) && (ratSpeed >= levelCustomization->getRatSpeedMax()) &&
            !proRunnerInProgress) {
            proRunnerInProgress = true;
        }

        // 2. ...and recover after a while
        if (ratCenter.y > earthCenter.y && proRunnerInProgress) {
            achievementTracker.proRunner();
        }
    }

    shadowRatHelper->doPhysicsCalculationStep();

    levelCustomization->customPhysicsStep(earthBody);
}

float LevelLayer::getEarthRadius() const
{
    float max_radius = 0;

    for (b2Fixture *f = earthBody->GetFixtureList(); f; f = f->GetNext()) {
        if (f->GetType() == b2Shape::e_circle) {
            max_radius = MAX(max_radius, f->GetShape()->m_radius);
        }
    }

    assert(max_radius > 0.0);
    return max_radius;
}

void LevelLayer::dropItem(float t)
{
    if (totalTime > nextItemDropTime) {
        nextItemDropTime += levelCustomization->getItemDropInterval(gameScore);
        doItemDrop();
    }
}

void LevelLayer::doItemDrop()
{
    LevelCustomization::ItemType itemType =
        levelCustomization->getDropItemType(ratSpeed, skullShieldCount < maxSkullShieldsCount);

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
    body->SetTransform(levelCustomization->getDropItemSpot(ratBody->GetPosition()),
                       rand_0_1() * 2 * M_PI);

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
    SoundHelper::getInstance().playEffectForItem(itemType);
    achievementTracker.itemCaught(itemType);

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

    if (itemType == LevelCustomization::BREAK) {
        breakItemEaten();
    }

    if (itemType == LevelCustomization::FRENZY) {
        frenzyItemEaten();
    }

    if (itemType == LevelCustomization::SHIELD) {
        shieldItemEaten();
    }
}

void LevelLayer::speedUpItemEaten()
{
    ratSpeed = b2Min(ratSpeed + levelCustomization->getRatSpeedStep(),
                     levelCustomization->getRatSpeedMax());

    adaptGameSpeed();

    animationHelper->playEyesAnimation(AnimationHelper::Eyes::DAZED);

    if (std::isgreaterequal(ratSpeed, levelCustomization->getRatSpeedMax())) {
        achievementTracker.maxSpeedReached();
    }

    if (std::isgreaterequal(ratSpeed, levelCustomization->getRatSpeedMax() -
                                          levelCustomization->getRatSpeedStep())) {
        SoundHelper::getInstance().playBackgroundMusic("fast_rocknroll");
    }
}

void LevelLayer::slowDownItemEaten()
{
    ratSpeed = b2Max(ratSpeed - levelCustomization->getRatSpeedStep(),
                     levelCustomization->getRatSpeedMin());

    adaptGameSpeed();

    animationHelper->playEyesAnimation(AnimationHelper::Eyes::SLEEPY);

    if (std::isless(ratSpeed,
                    levelCustomization->getRatSpeedMax() - levelCustomization->getRatSpeedStep())) {
        playBackgroundMusic(0.25);
    }
}

void LevelLayer::hoverItemEaten()
{
    isHovering = true;

    animationHelper->playHoveringAnimation();
    ratBody->SetGravityScale(-2);

    stopDroppingItems();

    getAnySpriteOnBody(ratBody)->runAction(Sequence::create(
        DelayTime::create(hoverDuration * 0.9),
        CallFunc::create([this]() { animationHelper->playRunningAnimation(ratSpeed); }),
        DelayTime::create(hoverDuration * 0.1), CallFunc::create([this]() {
            startDroppingItems();
            isHovering = false;
            ratBody->SetGravityScale(1.0);
        }),
        nullptr));
}

void LevelLayer::halveItemEaten()
{
    if (skullShieldCount > 0) {
        animationHelper->playEyesAnimation(AnimationHelper::Eyes::WINKING);

        updateRatShield(-1);

        return;
    }

    int halfScore = gameScore / 2;
    gameScore -= halfScore;

    scheduleUpdateScore();

    animationHelper->playEyesAnimation(AnimationHelper::Eyes::SAD);

    scoreLabel->runAction(Sequence::create(
        CallFunc::create([=]() { halvePointsParticleNode->resetSystem(); }), DelayTime::create(1.5),
        CallFunc::create([=]() { halvePointsParticleNode->stopSystem(); }), nullptr));
}

void LevelLayer::breakItemEaten()
{
    ratSpeed = levelCustomization->getRatSpeedMin();

    adaptGameSpeed();

    animationHelper->playEyesAnimation(AnimationHelper::Eyes::IRRITATED);

    playBackgroundMusic(0.25);
}

void LevelLayer::frenzyItemEaten()
{
    cheeseFrenzyParticleNode->resetSystem();
    frenzyGameScoreMultiplier = 5;

    scoreLabel->runAction(
        Sequence::create(CallFunc::create([=]() { cheeseFrenzyParticleNode->resetSystem(); }),
                         DelayTime::create(5.0), CallFunc::create([=]() {
                             frenzyGameScoreMultiplier = 1;
                             cheeseFrenzyParticleNode->stopSystem();
                         }),
                         nullptr));
}

void LevelLayer::shieldItemEaten() { updateRatShield(1); }

void LevelLayer::adaptGameSpeed()
{
    backgroundSpeedFunction(getRatSpeedLevel());

    if (!isHovering) {
        animationHelper->playRunningAnimation(ratSpeed);
    }
}

void LevelLayer::handleGameOver()
{
    achievementTracker.gameEnded();

    if (gameFinishedCallback) {
        gameFinishedCallback(gameScore);
    }
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

    if (itemType == LevelCustomization::BREAK) {
        return "item_break";
    }

    if (itemType == LevelCustomization::FRENZY) {
        return "item_frenzy";
    }

    if (itemType == LevelCustomization::SHIELD) {
        return "item_shield";
    }

    assert(false);

    return "";
}

void LevelLayer::initScoreLabel(int score)
{
    scoreLabel = DigitsPanel::createWithNumberOfDigits(6);

    addChild(scoreLabel, 1);

    float f = getAnySpriteOnBody(earthBody)->getScale();
    scoreLabel->setScale(0.35 * f);

    b2Vec2 pos = earthBody->GetWorldCenter();
    scoreLabel->setPosition(pos.x, pos.y);
    scoreLabel->animateToNumber(score);

    halvePointsParticleNode = ParticleSystemQuad::create("halve_points.plist");
    halvePointsParticleNode->setScale(5);
    halvePointsParticleNode->stopSystem();
    scoreLabel->addChild(halvePointsParticleNode, -1);

    // ---

    cheeseFrenzyParticleNode = ParticleSystemQuad::create("cheese_frenzy.plist");
    cheeseFrenzyParticleNode->setScale(5);
    cheeseFrenzyParticleNode->stopSystem();

    scoreLabel->addChild(cheeseFrenzyParticleNode, -1);
}

void LevelLayer::updateScore(float t)
{
    gameScore = std::min(gameScore, LevelCustomization::gameCompletedScore);

    scoreLabel->animateToNumber(gameScore);
    shadowRatHelper->scoreUpdated(gameScore);

    if (gameScore == LevelCustomization::gameCompletedScore && gameCompletedCallback) {
        gameCompletedCallback();
    }
}

void LevelLayer::calculateScore()
{
    float currentAngle = -earthRevoluteJoint->GetJointAngle();
    if (currentAngle - previousRevoluteJointAngle > 0.2) {
        gameScore += getGameScoreDelta();

        scheduleUpdateScore();

        previousRevoluteJointAngle = currentAngle;
    } else if (currentAngle < previousRevoluteJointAngle) {
        previousRevoluteJointAngle = currentAngle;
    }
}

void LevelLayer::scheduleUpdateScore()
{
    if (!isScheduled(schedule_selector(LevelLayer::updateScore))) {
        // do update of score label and shadow rats in next event loop
        // (to prevent re-entering box2d engine, since it may happen we try
        //  to add shadow rat's b2Body when world is still locked when handling
        //  collisions. and score gets updated as a result of collision
        //  with skull item.)
        scheduleOnce(schedule_selector(LevelLayer::updateScore), 0);
    }
}

spine::SkeletonAnimation *LevelLayer::getRatAnimation()
{
    spine::SkeletonAnimation *skeletonAnimation =
        dynamic_cast<spine::SkeletonAnimation *>(getAnySpriteOnBody(ratBody));
    assert(skeletonAnimation);

    return skeletonAnimation;
}

void LevelLayer::updateRatShield(int delta)
{
    assert(delta == -1 || delta == 1);

    if (delta == 1 && skullShieldCount == 0) {
        animationHelper->playShowShieldAnimation();
    }

    if (delta == -1 && skullShieldCount == 1) {
        animationHelper->playHideShieldAnimation();
    }

    skullShieldCount += delta;

    assert(skullShieldCount >= 0 && skullShieldCount <= maxSkullShieldsCount);

    if (skullShieldCount > 0) {
        std::string attachmentName = "helmet0" + std::to_string(skullShieldCount);
        getRatAnimation()->setAttachment("helmet", attachmentName);
    }
}

int LevelLayer::getGameScoreDelta() const
{
    float speedRatio =
        (ratSpeed - levelCustomization->getRatSpeedMin()) /
        (levelCustomization->getRatSpeedMax() - levelCustomization->getRatSpeedMin());

    int gameScoreDelta =
        1 +
        std::floor(speedRatio * 2 + (ratSpeed == levelCustomization->getRatSpeedMax() ? 1 : 0) + 0.5);
    /* note: lround is not supported on Android */

    gameScoreDelta *= frenzyGameScoreMultiplier;

    return gameScoreDelta;
}
