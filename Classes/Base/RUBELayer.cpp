//  Author: Chris Campbell - www.iforce2d.net
//  -----------------------------------------
//
//  RUBELayer
//
//  See header file for description.
//

#include "RUBELayer.h"
#include "b2dJson.h"
#include "b2dJsonImage.h"

using namespace std;
using namespace cocos2d;

// This is called after the Box2D world has been loaded, and while the b2dJson information
// is still available to do extra loading. Here is where we load the images.
void RUBELayer::afterLoadProcessing(b2dJson *json)
{
    // fill a vector with all images in the RUBE scene
    std::vector<b2dJsonImage *> b2dImages;
    json->getAllImages(b2dImages);

    // loop through the vector, create Sprites for each image and store them in m_imageInfos
    for (int i = 0; i < b2dImages.size(); i++) {
        b2dJsonImage *img = b2dImages[i];

        CCLOG("Loading image: %s", img->file.c_str());

        // try to load the sprite image, and ignore if it fails
        Sprite *sprite = Sprite::create(img->file.c_str());
        if (!sprite)
            continue;

        // add the sprite to this layer and set the render order
        addChild(sprite);
        reorderChild(sprite, img->renderOrder); // watch out - RUBE render order is float but
                                                // cocos2d uses integer (why not float?)

        // these will not change during simulation so we can set them now
        sprite->setFlippedX(img->flip);
        sprite->setColor(Color3B(img->colorTint[0], img->colorTint[1], img->colorTint[2]));
        sprite->setOpacity(img->colorTint[3]);
        sprite->setScale(img->scale / sprite->getContentSize().height);

        // create an info structure to hold the info for this image (body and position etc)
        RUBEImageInfo *imgInfo = new RUBEImageInfo;
        imgInfo->sprite = sprite;
        imgInfo->name = img->name;
        imgInfo->file = img->file;
        imgInfo->body = img->body;
        imgInfo->scale = img->scale;
        imgInfo->aspectScale = img->aspectScale;
        imgInfo->angle = img->angle;
        imgInfo->center = Vec2(img->center.x, img->center.y);
        imgInfo->opacity = img->opacity;
        imgInfo->flip = img->flip;
        for (int n = 0; n < 4; n++)
            imgInfo->colorTint[n] = img->colorTint[n];

        // add the info for this image to the list
        m_imageInfos.insert(imgInfo);
    }

    // start the images at their current positions on the physics bodies
    setImagePositionsFromPhysicsBodies();
}

// This method should undo anything that was done by afterLoadProcessing, and make sure
// to call the superclass method so it can do the same
void RUBELayer::clear()
{
    for (set<RUBEImageInfo *>::iterator it = m_imageInfos.begin(); it != m_imageInfos.end(); ++it) {
        RUBEImageInfo *imgInfo = *it;
        removeChild(imgInfo->sprite, true);
    }
    m_imageInfos.clear();

    BasicRUBELayer::clear();
}

// Standard Cocos2d method. Call the super class to step the physics world, and then
// move the images to match the physics body positions
void RUBELayer::update(float dt)
{
    // superclass will Step the physics world
    BasicRUBELayer::update(dt);
    setImagePositionsFromPhysicsBodies();
}

// Move all the images to where the physics engine says they should be
void RUBELayer::setImagePositionsFromPhysicsBodies()
{
    for (set<RUBEImageInfo *>::iterator it = m_imageInfos.begin(); it != m_imageInfos.end(); ++it) {
        RUBEImageInfo *imgInfo = *it;
        Point pos = imgInfo->center;
        float angle = -imgInfo->angle;

        if (imgInfo->body) {
            if (imgInfo->name == "rat") // special handling for rat image
            {
                // adapt position
                b2Vec2 v = imgInfo->body->GetPosition();
                b2Fixture *fixture = imgInfo->body->GetFixtureList();
                assert(fixture); // rat model is just a ball
                float radius = fixture->GetShape()->m_radius;
                pos = Vec2(v.x, v.y + 2 * radius);

                // rotate texture as it was attached to hook point
                b2Vec2 hookPoint(0, 15);

                angle += acos(b2Dot(hookPoint, hookPoint - v) / (hookPoint - v).Length() /
                              hookPoint.Length());

                if (v.x > 0) {
                    angle = -angle;
                }
            } else {
                // need to rotate image local center by body angle
                b2Vec2 localPos(pos.x, pos.y);
                b2Rot rot(imgInfo->body->GetAngle());
                localPos = b2Mul(rot, localPos) + imgInfo->body->GetPosition();
                pos.x = localPos.x;
                pos.y = localPos.y;
                angle += -imgInfo->body->GetAngle();
            }
        }

        imgInfo->sprite->setRotation(CC_RADIANS_TO_DEGREES(angle));

        imgInfo->sprite->setPosition(pos);
    }
}

// Remove one body and any images is had attached to it from the layer
void RUBELayer::removeBodyFromWorld(b2Body *body)
{
    // destroy the body in the physics world
    m_world->DestroyBody(body);

    // go through the image info array and remove all sprites that were attached to the body we just
    // deleted
    vector<RUBEImageInfo *> imagesToRemove;
    for (set<RUBEImageInfo *>::iterator it = m_imageInfos.begin(); it != m_imageInfos.end(); ++it) {
        RUBEImageInfo *imgInfo = *it;
        if (imgInfo->body == body) {
            removeChild(imgInfo->sprite, true);
            imagesToRemove.push_back(imgInfo);
        }
    }

    // also remove the infos for those images from the image info array
    for (int i = 0; i < imagesToRemove.size(); i++)
        m_imageInfos.erase(imagesToRemove[i]);
}

// Remove one image from the layer
void RUBELayer::removeImageFromWorld(RUBEImageInfo *imgInfo)
{
    removeChild(imgInfo->sprite, true);
    m_imageInfos.erase(imgInfo);
}

Sprite *RUBELayer::getAnySpriteOnBody(b2Body *body)
{
    for (set<RUBEImageInfo *>::iterator it = m_imageInfos.begin(); it != m_imageInfos.end(); ++it) {
        RUBEImageInfo *imgInfo = *it;
        if (imgInfo->body == body)
            return imgInfo->sprite;
    }
    return NULL;
}

Sprite *RUBELayer::getSpriteWithImageName(std::string name)
{
    for (set<RUBEImageInfo *>::iterator it = m_imageInfos.begin(); it != m_imageInfos.end(); ++it) {
        RUBEImageInfo *imgInfo = *it;
        if (imgInfo->name == name)
            return imgInfo->sprite;
    }
    return NULL;
}
