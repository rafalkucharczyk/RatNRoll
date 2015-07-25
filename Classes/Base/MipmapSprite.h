#ifndef MIPMAP_SPRITE_H
#define MIPMAP_SPRITE_H

#include <2d/CCSprite.h>

class MipmapSprite : public cocos2d::Sprite
{
  public:
    static Sprite *create(const std::string &filename);
};

#endif /* MIPMAP_SPRITE_H */
