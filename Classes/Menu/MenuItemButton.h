#ifndef __MENU_ITEM_BUTTON_H__
#define __MENU_ITEM_BUTTON_H__

#include "cocos2d.h"

#include <string>

class MenuItemButton : public cocos2d::MenuItemImage
{
  public:
    static MenuItemButton *create(const std::string &imagePath,
                                  const cocos2d::ccMenuCallback &callback);

    void activate() override;
    void selected() override;
    void unselected() override;

  private:
    float originalScale;
    static const int zoomActionTag = 112233;
};

#endif // __MENU_ITEM_BUTTON_H__
