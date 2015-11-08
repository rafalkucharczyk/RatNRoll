//  Author: Chris Campbell - www.iforce2d.net
//  -----------------------------------------
//
//  BasicRUBELayer
//
//  This class extends Layer to load in a RUBE scene file on init.
//  It uses the debug draw display to show the scene, and  does not
//  load any images. The touch methods are used to zoom, pan, and to
//  create a mouse joint to drag dynamic bodies around.
//
//  This class is mostly here to keep the first example simple, and
//  concentrate on getting positions and scales correct before any
//  images are involved. In most cases you would subclass RUBELayer
//  to make a more useful layer.
//
//  The scene file to load, and the initial position and scale are
//  given by methods which should be overridden in subclasses.
//
//  The position of the layer is set with setPosition(Point)
//  and specifies the location on screen where 0,0 in the physics world
//  will be located, in pixels. Hence you can check anytime where the
//  (0,0) point of the physics world is with getPosition()
//
//  The scale of the layer is the number of pixels for one physics unit.
//  Eg. if the screen is 320 pixels high and you want it to be 10 units
//  high in the physics world, the scale would be 32. You can set this
//  with [self setScale:(float)] and check it with [self scale].
//
//  This class provides the screenToWorld and worldToScreen methods
//  which are invaluable when converting locations between screen and
//  physics world coordinates.
//

#include "cocos2d.h"
#include <Box2D/Box2D.h>
#include "Box2DDebugDraw.h"

#ifndef BASIC_RUBE_LAYER
#define BASIC_RUBE_LAYER

class b2dJson;

class BasicRUBELayer : public cocos2d::Layer
{
  protected:
    b2World *m_world;                  // the physics world
    Box2DDebugDraw *m_debugDraw;       // used to draw debug data
    cocos2d::DrawNode *m_drawNode;     // used to draw debug data
    b2MouseJoint *m_mouseJoint;        // used when dragging bodies around
    b2Body *m_mouseJointGroundBody;    // the other body for the mouse joint (static, no fixtures)
    cocos2d::Touch *m_mouseJointTouch; // keep track of which touch started the mouse joint

  public:
    BasicRUBELayer();
    virtual ~BasicRUBELayer();

    virtual bool init(); // virtual functions cannot be used in the constructor, but we want to
                         // allow some customization from subclasses

    // override this in subclasses to specify which .json files to load
    virtual std::list<std::string> getFilenames() = 0;
    // override this in subclasses to set the inital view position
    virtual cocos2d::Point initialWorldOffset() = 0;
    // override this in subclasses to set the initial view scale
    virtual float initialWorldScale() = 0;

    virtual void loadWorld(); // attempts to load the world from the .json file given by getFilename
    virtual void afterLoadProcessing(b2dJson *json); // override this in a subclass to do something
                                                     // else after loading the world (before
                                                     // discarding the JSON info)
    virtual void clear(); // undoes everything done by loadWorld and afterLoadProcessing, so that
                          // they can be safely called again

    virtual b2Vec2 screenToWorld(cocos2d::Point screenPos); // converts a position in screen pixels
                                                            // to a location in the physics world
    virtual cocos2d::Point worldToScreen(
        b2Vec2 worldPos); // converts a location in the physics world to a position in screen pixels

    virtual void update(float dt); // standard Cocos2d layer method
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform,
                      uint32_t transformUpdated); // standard Cocos2d layer method

    void onTouchesBegan(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *unused_event);
    void onTouchesMoved(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *unused_event);
    void onTouchesEnded(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *unused_event);
    void onTouchesCancelled(const std::vector<cocos2d::Touch *> &touches,
                            cocos2d::Event *unused_event);

    b2Fixture *getTouchedFixture(
        cocos2d::Touch *touch); // returns the first fixture found under the touch location

    virtual bool allowPinchZoom(); // return false from this function to prevent pinch zoom and pan

    virtual bool drawDebugData() { return true; }

    // override to enable touches for specific fixtures only
    virtual bool isFixtureTouchable(b2Fixture *fixture) { return true; }
};

#endif /* BASIC_RUBE_LAYER */
