//  Author: Chris Campbell - www.iforce2d.net
//  -----------------------------------------
//
//  RUBELayer
//
//  Extends BasicRUBELayer and also loads images. This is the class
//  you would typically extend to make your own layers.
//

#ifndef RUBE_LAYER
#define RUBE_LAYER

#include "BasicRUBELayer.h"

class b2dJsonImage;

//
//  RUBEImageInfo
//
//  Holds information about one image in the layer, most importantly
//  the body it is attached to and its position relative to that body.
//
//  When the body is moved by the physics engine, this information is
//  used to place the image in the correct position to match the physics.
//  If the body is NULL, the position is relative to 0,0 and angle zero.
//
struct RUBEImageInfo {

    cocos2d::Node *sprite; // the image
    std::string name;      // the name of the image
    std::string file;      // the file the image was loaded from (or name of skeleton animation)
    class b2Body *body;    // the body this image is attached to (can be NULL)
    float scale;           // a scale of 1 means the image is 1 physics unit high
    float aspectScale;     // modify the natural aspect of the image
    float angle;           // 'local angle' - relative to the angle of the body
    cocos2d::Point center; // 'local center' - relative to the position of the body
    float opacity;         // 0 - 1
    bool flip;             // horizontal flip
    int colorTint[4];      // 0 - 255 RGBA values
    float renderOrder;
};

class RUBELayer : public BasicRUBELayer
{
  private:
    void fillRUBEImageInfoFromb2dJsonImage(const b2dJsonImage &jsonImage, RUBEImageInfo &imageInfo);
    Node *createNodeFromRubeImageInfo(RUBEImageInfo *imageInfo);

  protected:
    std::set<RUBEImageInfo *>
        m_imageInfos; // holds some information about images in the scene, most importantly the
                      //     body they are attached to and their position relative to that body

    void addNodeFromRubeImageInfo(RUBEImageInfo *imageInfo);
    void duplicateImageForBody(const std::string &name, b2Body *body);

  public:
    ~RUBELayer();

    virtual void afterLoadProcessing(b2dJson *json); // overrides base class
    virtual void clear();                            // overrides base class

    // sets position of individual sprite based on associated physics body
    void setImagePositionFromPhysicsBody(RUBEImageInfo *imageInfo);
    void setImagePositionsFromPhysicsBodies(); // called every frame to move the images to the
                                               // correct position when bodies move

    virtual bool setCustomImagePositionsFromPhysicsBodies(const RUBEImageInfo *imageInfo,
                                                          cocos2d::Point &position, float &angle);

    virtual void update(float dt); // standard Cocos2d function

    void removeBodyFromWorld(b2Body *body); // removes a body and its images from the layer
    void removeImageFromWorld(RUBEImageInfo *imgInfo); // removes an image from the layer

    cocos2d::Node *getAnySpriteOnBody(b2Body *body); // returns the first sprite found attached to
                                                     // the given body, or nil if there are none
    cocos2d::Node *getSpriteWithImageName(std::string name); // returns the first sprite found
                                                             // with the give name (as named in
                                                             // the RUBE scene) or nil if there is
                                                             // none
};

#endif /* RUBE_LAYER */
