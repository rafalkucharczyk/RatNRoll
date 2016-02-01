#include "InitialLayer.h"

#include "AssetsPreloader.h"
#include "OsWrapper.h"

USING_NS_CC;

InitialLayer::InitialLayer()
    : menuHelper(OsWrapper::getInitMenuLayout(),
          std::bind(&InitialLayer::itemClicked, this, std::placeholders::_1))
{
}

bool InitialLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    addChild(menuHelper.getMenu());

    return true;
}

void InitialLayer::itemClicked(int itemIndex)
{
    CCLOG("%s: itemIndex: %d", __func__, itemIndex);
    switch (itemIndex) {
        case 0:
            if (playCallback) {
                playCallback();
            }
            break;
        case 1:
            if (settingsCallback) {
                settingsCallback();
            }
            break;
        case 2:
        case 3:
            if (gameCenterIsSignedInCallback && gameCenterActionCallback) {
                bool signedIn = gameCenterIsSignedInCallback();
                CCLOG("%s: signedIn : %d", __func__, signedIn);

                if (!signedIn) {
                    CCLOG("%s: not signed in, running action!");
                    menuHelper.runActionFor(itemIndex, 1);
                } else {
                    CCLOG("%s: signed in, not running action!");
                }


                gameCenterActionCallback(signedIn, itemIndex);
            }
            break;
        default:
            break;
    }
}
