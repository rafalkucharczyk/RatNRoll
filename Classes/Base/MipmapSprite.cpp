#include "MipmapSprite.h"

USING_NS_CC;

Sprite *MipmapSprite::create(const std::string &filename)
{
    Sprite *sprite = Sprite::create(filename);

    if (!sprite->getTexture()->hasMipmaps()) {
        sprite->getTexture()->generateMipmap();
        Texture2D::TexParams texParams = {GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE,
                                          GL_CLAMP_TO_EDGE};
        sprite->getTexture()->setTexParameters(texParams);
    }

    return sprite;
}
