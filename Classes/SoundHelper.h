#ifndef __SOUND_HELPER_H__
#define __SOUND_HELPER_H__

#include "LevelCustomization.h"

#include "MenuHelper.h"

class SoundHelper
{
  public:
    static SoundHelper &getInstance();

    void init();
    void playEffectForItem(LevelCustomization::ItemType itemType);

  private:
    SoundHelper(){};

    static SoundHelper *instance;
};

#endif // __SOUND_HELPER_H__
