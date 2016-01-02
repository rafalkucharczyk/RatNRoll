#include "ToString.h"
#include "DigitsPanel.h"

USING_NS_CC;

DigitsPanel *DigitsPanel::createWithNumberOfDigits(int numberOfDigits)
{
    DigitsPanel *ret = new (std::nothrow) DigitsPanel();

    std::vector<int> digits(numberOfDigits);
    std::fill(digits.begin(), digits.end(), 0);

    if (ret && ret->initWithDigits(digits)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

DigitsPanel *DigitsPanel::createWithNumber(int number)
{
    DigitsPanel *ret = new (std::nothrow) DigitsPanel();

    if (ret && ret->initWithDigits(intToDigits(number))) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

void DigitsPanel::animateToNumber(int number)
{
    std::vector<int> newDigits = sanitizeNumber(number);

    for (int i = 0; i < digitNodes.size(); i++) {
        std::vector<std::string> animations =
            getAnimationsFromToDigit(digitNodes[i].first, newDigits[i]);

        if (animations.empty()) {
            continue;
        }

        digitNodes[i].first = newDigits[i];

        digitNodes[i].second->setTimeScale(5);

        digitNodes[i].second->runAction(CallFuncN::create([=](Node *node) {
            digitNodes[i].second->setCompleteListener([=](int trackIndex, int) {
                if (trackIndex < animations.size() - 1) {
                    digitNodes[i].second->setAnimation(trackIndex + 1, animations[trackIndex + 1],
                                                       false);
                };
            });
            digitNodes[i].second->setAnimation(0, animations[0], false);
        }));
    }
}

void DigitsPanel::setNumber(int number)
{
    std::vector<int> newDigits = sanitizeNumber(number);

    for (int i = 0; i < digitNodes.size(); i++) {
        digitNodes[i].first = newDigits[i];
        setDigitOnNode(*digitNodes[i].second, newDigits[i]);
    }
}

bool DigitsPanel::initWithDigits(std::vector<int> digits)
{
    float totalWidth, spacingWidth;
    std::tie(totalWidth, spacingWidth) = getTotalWidth(digits.size());

    float x = -(totalWidth / 2);

    std::srand(std::time(0));

    for (auto i = digits.begin(); i != digits.end(); i++) {
        auto digitNode = createDigitNode();
        digitNode->updateWorldTransform();

        digitNode->setPositionX(x);

        // always create slightly different layout of digits
        digitNode->setPositionY(rand_minus1_1() * 20);
        digitNode->setRotation(rand_minus1_1() * 10);

        x += digitNode->getBoundingBox().size.width + spacingWidth;

        setDigitOnNode(*digitNode, *i);
        digitNodes.push_back(std::make_pair(*i, digitNode));

        boundingBox.merge(digitNode->getBoundingBox());

        addChild(digitNode);
    }

    return true;
}

Rect DigitsPanel::getBoundingBox() const { return boundingBox; }

spine::SkeletonAnimation *DigitsPanel::createDigitNode() const
{
    return spine::SkeletonAnimation::createWithFile("animations/digits/skeleton.json",
                                                    "animations/digits/skeleton.atlas");
}

std::tuple<float, float> DigitsPanel::getTotalWidth(int numberOfDigits) const
{
    auto digitNode = createDigitNode();

    digitNode->updateWorldTransform();
    float digitNodeWidth = digitNode->getBoundingBox().size.width;
    float spacingWidth = digitNodeWidth * 0.04;

    return std::make_tuple((numberOfDigits - 1) * (digitNodeWidth + spacingWidth), spacingWidth);
}

void DigitsPanel::setDigitOnNode(spine::SkeletonAnimation &digitNode, int digit) const
{
    assert(digit >= 0 && digit <= 9);

    digitNode.setAttachment("top_bg", std::to_string(digit) + "_top");
    digitNode.setAttachment("top", std::to_string(digit) + "_top");
    digitNode.setAttachment("bot_bg", std::to_string(digit) + "_bot");
    digitNode.setAttachment("bot", std::to_string(digit) + "_bot");
}

std::vector<int> DigitsPanel::intToDigits(int number)
{
    std::vector<int> result;

    for (int i = 10; i >= 0; i--) {
        int d = std::pow(10, i);

        int v = number / d;

        // skip initial 0s
        if (v == 0 && result.empty()) {
            continue;
        }

        result.push_back(v);

        number -= d * result.back();
    }

    return result;
}

std::vector<std::string> DigitsPanel::getAnimationsFromToDigit(int fromDigit, int toDigit)
{
    assert(fromDigit >= 0 && fromDigit <= 9);
    assert(toDigit >= 0 && toDigit <= 9);

    std::vector<std::string> result;

    auto makeAnimationName = [](int i, int j) -> std::string {
        return std::to_string(i) + "_" + std::to_string(j) + ".x";
    };

    if (toDigit > fromDigit) {
        for (int i = fromDigit; i < toDigit; i++) {
            result.push_back(makeAnimationName(i, i + 1));
        }
    } else if (toDigit < fromDigit) {
        for (int i = fromDigit; i < 9; i++) {
            result.push_back(makeAnimationName(i, i + 1));
        }
        result.push_back(makeAnimationName(9, 0));
        for (int i = 0; i < toDigit; i++) {
            result.push_back(makeAnimationName(i, i + 1));
        }
    }

    return result;
}

std::vector<int> DigitsPanel::sanitizeNumber(int number)
{
    std::vector<int> digits = intToDigits(number);

    std::vector<int> newDigits(digitNodes.size());

    if (digits.size() > newDigits.size()) {
        CCLOG("Number %d has more digits than panel and will be truncated!", number);
    }

    std::copy_backward(digits.begin(), digits.end(), newDigits.end());

    return newDigits;
}
