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

        // create an info structure to hold the info for this image (body and position etc)
        RUBEImageInfo *imgInfo = new RUBEImageInfo;

        fillRUBEImageInfoFromb2dJsonImage(*img, *imgInfo);

        addSpriteFromRubeImageInfo(imgInfo);
    }

    // start the images at their current positions on the physics bodies
    setImagePositionsFromPhysicsBodies();
}

void RUBELayer::fillRUBEImageInfoFromb2dJsonImage(const b2dJsonImage &jsonImage,
                                                  RUBEImageInfo &imageInfo)
{
    imageInfo.sprite = NULL;
    imageInfo.name = jsonImage.name;
    imageInfo.file = jsonImage.file;
    imageInfo.body = jsonImage.body;
    imageInfo.scale = jsonImage.scale;
    imageInfo.aspectScale = jsonImage.aspectScale;
    imageInfo.angle = jsonImage.angle;
    imageInfo.center = Vec2(jsonImage.center.x, jsonImage.center.y);
    imageInfo.opacity = jsonImage.opacity;
    imageInfo.flip = jsonImage.flip;
    imageInfo.renderOrder = jsonImage.renderOrder;
    for (int n = 0; n < 4; n++) {
        imageInfo.colorTint[n] = jsonImage.colorTint[n];
    }
}

void RUBELayer::addSpriteFromRubeImageInfo(RUBEImageInfo *imageInfo)
{
    m_imageInfos.insert(imageInfo);

    Sprite *sprite = Sprite::create(imageInfo->file.c_str());
    imageInfo->sprite = sprite;

    assert(sprite);

    addChild(sprite);
    reorderChild(sprite, imageInfo->renderOrder);

    sprite->setFlippedX(imageInfo->flip);
    sprite->setColor(
        Color3B(imageInfo->colorTint[0], imageInfo->colorTint[1], imageInfo->colorTint[2]));
    sprite->setOpacity(imageInfo->colorTint[3]);
    sprite->setScale(imageInfo->scale / sprite->getContentSize().height);
}

void RUBELayer::duplicateImageForBody(const std::string &name, b2Body *body)
{
    RUBEImageInfo *imageInfo = NULL;

    for (std::set<RUBEImageInfo *>::const_iterator i = m_imageInfos.begin();
         i != m_imageInfos.end(); i++) {
        if ((*i)->name == name) {
            imageInfo = *i;
            break;
        }
    }

    assert(imageInfo);

    RUBEImageInfo *newImageInfo = new RUBEImageInfo(*imageInfo);
    newImageInfo->sprite = nullptr;
    newImageInfo->body = body;

    addSpriteFromRubeImageInfo(newImageInfo);
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

                float d = acos(b2Dot(hookPoint, hookPoint - v) / (hookPoint - v).Length() /
                               hookPoint.Length());

                if (!isnan(d)) {
                    angle += d;

                    if (v.x > 0) {
                        angle = -angle;
                    }
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