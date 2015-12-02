#include "MenuLabel.h"

#include "MenuHelper.h"

USING_NS_CC;

MenuLabel::MenuLabel() : label(nullptr) {}

MenuLabel *MenuLabel::create(const std::string &text, const cocos2d::Vec2 &position, float size)
{
    MenuLabel *ret = new (std::nothrow) MenuLabel();

    if (ret && ret->init()) {
        ret->autorelease();

        ret->addLabel(text, position, size);
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

float MenuLabel::getTotalSize() const
{
    assert(label);

    Vec2 visibleSize = Director::getInstance()->getVisibleSize();

    return label->getBoundingBox().size.height / visibleSize.y;
}

void MenuLabel::addLabel(const std::string &text, const cocos2d::Vec2 &position, float size)
{
    auto linesCount = std::count(text.begin(), text.end(), '\n') + 1;

    label = Label::createWithTTF(text, "fonts/rat.ttf", 20 * MenuHelper::getContentScaleFactor(),
                                 Size::ZERO, TextHAlignment::CENTER);

    label->setColor(Color3B::BLACK);

    auto labelHeight = 1.1 * size * linesCount;
    MenuHelper::positionNode(*label, position, labelHeight);

    addChild(label);
}
