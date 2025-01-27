#ifndef BOX2DDEBUGDRAW_H
#define BOX2DDEBUGDRAW_H

#include "cocos2d.h"
#include <Box2D/Box2D.h>

struct b2AABB;

class Box2DDebugDraw : public b2Draw
{
  public:
    Box2DDebugDraw(cocos2d::DrawNode *drawNode, float aRatio);

    void DrawPolygon(const b2Vec2 *aVertices, int32 aVertexCount, const b2Color &aColor);
    void DrawSolidPolygon(const b2Vec2 *aVertices, int32 aVertexCount, const b2Color &aColor);
    void DrawCircle(const b2Vec2 &aCenter, float32 aRadius, const b2Color &aColor);
    void DrawSolidCircle(const b2Vec2 &aCenter, float32 aRadius, const b2Vec2 &aAxis,
                         const b2Color &aColor);
    void DrawSegment(const b2Vec2 &aP1, const b2Vec2 &aP2, const b2Color &aColor);
    void DrawTransform(const b2Transform &aXf);
    void DrawPoint(const b2Vec2 &aP, float32 aSize, const b2Color &aColor);
    void DrawString(int aX, int aY, const char *aString, ...);
    void DrawAABB(b2AABB *aAabb, const b2Color &aColor);

  private:
    static const int DEBUG_DRAW_MAX_VERTICES = 64;
    static const int DEBUG_DRAW_CIRCLE_SEGMENTS = 32;
    float mRatio;
    cocos2d::Point mVertices[DEBUG_DRAW_MAX_VERTICES];
    cocos2d::DrawNode *drawNode;
};

#endif /* BOX2DDEBUGDRAW_H */
