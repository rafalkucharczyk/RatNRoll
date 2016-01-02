#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <cocos2d.h>
#include "FacebookHelper.h"

FacebookHelper::FacebookHelper()
{
    CCLOG("%s: not implemented for android!", __func__);
}

FacebookHelper::~FacebookHelper()
{
    CCLOG("%s: not implemented for android!", __func__);
}

void FacebookHelper::showLikeControl(int x, int y, int width, const std::string &objectId)
{
    CCLOG("%s: not implemented for android!", __func__);
}

void FacebookHelper::hideLikeControl()
{
    CCLOG("%s: not implemented for android!", __func__);
}
#endif /* CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID */
