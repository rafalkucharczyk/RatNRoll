#ifndef __DIGITS_PANEL_H__
#define __DIGITS_PANEL_H__

#include <cocos2d.h>
#include <spine/spine-cocos2dx.h>

#include <vector>
#include <tuple>

class DigitsPanel : public cocos2d::Node
{
  public:
    // creates panel with specified number of digits, all initially set to '0'
    static DigitsPanel *createWithNumberOfDigits(int numberOfDigits);

    // creates panel initialized with specified number
    static DigitsPanel *createWithNumber(int number);

    // sets (and animates to) given number
    void setNumber(int number);

  private:
    bool initWithDigits(std::vector<int> digits);

    virtual cocos2d::Rect getBoundingBox() const override;

  private:
    spine::SkeletonAnimation *createDigitNode() const;
    std::tuple<float, float> getTotalWidth(int numberOfDigits) const;
    void setDigitOnNode(spine::SkeletonAnimation &digitNode, int digit) const;
    static std::vector<int> intToDigits(int number);
    static std::vector<std::string> getAnimationsFromToDigit(int fromDigit, int toDigit);

  private:
    std::vector<std::pair<int, spine::SkeletonAnimation *>> digitNodes;
    cocos2d::Rect boundingBox;
};

#endif // __DIGITS_PANEL_H__
