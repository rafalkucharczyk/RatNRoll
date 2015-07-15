#include "MenuItemButton.h"

USING_NS_CC;

MenuItemButton *MenuItemButton::create(const std::string &imagePath, const ccMenuCallback &callback)
{
    MenuItemButton *ret = new (std::nothrow) MenuItemButton();
    if (ret && ret->initWithNormalImage(imagePath, "", "", callback)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

void MenuItemButton::activate()
{
    if (!_enabled) {
        return;
    }

    stopAllActions();
    setScale(originalScale);
    MenuItemImage::activate();
}

void MenuItemButton::selected()
{
    if (!_enabled) {
        return;
    }

    MenuItemImage::selected();

    Action *action = getActionByTag(zoomActionTag);
    if (action) {
        _normalImage->stopAction(action);
    } else {
        originalScale = this->getScale();
    }

    Action *zoomAction = ScaleTo::create(0.1f, originalScale * 1.1f);
    zoomAction->setTag(zoomActionTag);
    runAction(zoomAction);
}

void MenuItemButton::unselected()
{
    if (!_enabled) {
        return;
    }

    MenuItemImage::unselected();
    stopActionByTag(zoomActionTag);
    Action *zoomAction = ScaleTo::create(0.1f, originalScale);
    zoomAction->setTag(zoomActionTag);
    runAction(zoomAction);
}
