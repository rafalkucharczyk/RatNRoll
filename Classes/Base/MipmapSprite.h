#ifndef __MIPMAP_SPRITE_H__
#define __MIPMAP_SPRITE_H__

#include <2d/CCSprite.h>

class MipmapSprite : public cocos2d::Sprite
{
  public:
    static Sprite *create(const std::string &filename);
};

#endif // __MIPMAP_SPRITE_H__
