
#include "Box2DDebugDraw.h"

using namespace cocos2d;

Box2DDebugDraw::Box2DDebugDraw(DrawNode *_drawNode, float aRatio) : mRatio(aRatio)
{
    drawNode = _drawNode;
}

void Box2DDebugDraw::DrawPolygon(const b2Vec2 *aVertices, int32 aVertexCount, const b2Color &aColor)
{
    CCLOG("adsf");
    for (int i = 0; i < DEBUG_DRAW_MAX_VERTICES && i < aVertexCount; i++)
        mVertices[i].setPoint(mRatio * aVertices[i].x, mRatio * aVertices[i].y);

    Color4F color(aColor.r, aColor.g, aColor.b, 1);
    drawNode->drawPoly((Vec2 *)aVertices, aVertexCount, true, color);
}

void Box2DDebugDraw::DrawSolidPolygon(const b2Vec2 *aVertices, int32 aVertexCount,
                                      const b2Color &aColor)
{
    for (int i = 0; i < DEBUG_DRAW_MAX_VERTICES && i < aVertexCount; i++)
        mVertices[i].setPoint(mRatio * aVertices[i].x, mRatio * aVertices[i].y);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Color4F color(aColor.r, aColor.g, aColor.b, 0.5);
    drawNode->drawSolidPoly((Vec2 *)aVertices, aVertexCount, color);

    color.a = 1;
    drawNode->drawPoly((Vec2 *)aVertices, aVertexCount, true, color);
}

void Box2DDebugDraw::DrawCircle(const b2Vec2 &aCenter, float32 aRadius, const b2Color &aColor)
{
    Color4F color(aColor.r, aColor.g, aColor.b, 1);
    drawNode->drawCircle(Vec2(mRatio * aCenter.x, mRatio * aCenter.y), aRadius, 0,
                         DEBUG_DRAW_CIRCLE_SEGMENTS, false, color);
}

void Box2DDebugDraw::DrawSolidCircle(const b2Vec2 &aCenter, float32 aRadius, const b2Vec2 &aAxis,
                                     const b2Color &aColor)
{
    const float coef = 2.0f * (float)M_PI / DEBUG_DRAW_CIRCLE_SEGMENTS;

    mVertices[0].setPoint(mRatio * aCenter.x, mRatio * aCenter.y);
    for (unsigned int i = 0; i <= DEBUG_DRAW_CIRCLE_SEGMENTS; i++) {
        float rads = i * coef;
        GLfloat j = aRadius * cosf(rads) + mRatio * aCenter.x;
        GLfloat k = aRadius * sinf(rads) + mRatio * aCenter.y;

        mVertices[i].setPoint(j, k);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Color4F color(aColor.r, aColor.g, aColor.b, 0.5);
    drawNode->drawSolidPoly(mVertices, DEBUG_DRAW_CIRCLE_SEGMENTS, color);

    color.a = 1;
    drawNode->drawCircle(Vec2(mRatio * aCenter.x, mRatio * aCenter.y), aRadius, 0,
                         DEBUG_DRAW_CIRCLE_SEGMENTS, false, color);

    b2Vec2 p = aCenter + aRadius * aAxis;
    drawNode->drawLine(Vec2(aCenter.x, aCenter.y), Vec2(p.x, p.y), color);
}

void Box2DDebugDraw::DrawSegment(const b2Vec2 &aP1, const b2Vec2 &aP2, const b2Color &aColor)
{
    Color4F color(aColor.r, aColor.g, aColor.b, 1);
    drawNode->drawLine(Vec2(aP1.x, aP1.y), Vec2(aP2.x, aP2.y), color);
}

void Box2DDebugDraw::DrawTransform(const b2Transform &aXf)
{
    b2Vec2 p1 = aXf.p, p2;
    const float32 k_axisScale = 0.4f;

    p2 = p1 + k_axisScale * aXf.q.GetXAxis();
    DrawSegment(p1, p2, b2Color(1, 0, 0));

    p2 = p1 + k_axisScale * aXf.q.GetYAxis();
    DrawSegment(p1, p2, b2Color(0, 1, 0));
}

void Box2DDebugDraw::DrawPoint(const b2Vec2 &aP, float32 aSize, const b2Color &aColor)
{
    Color4F color(aColor.r, aColor.g, aColor.b, 1);
    drawNode->drawPoint(Vec2(mRatio * aP.x, mRatio * aP.y), aSize, color);
}

void Box2DDebugDraw::DrawString(int aX, int aY, const char *aString, ...) {}

void Box2DDebugDraw::DrawAABB(b2AABB *aAabb, const b2Color &aColor)
{
    mVertices[0].setPoint(aAabb->lowerBound.x * mRatio, aAabb->lowerBound.y * mRatio);
    mVertices[1].setPoint(aAabb->upperBound.x * mRatio, aAabb->lowerBound.y * mRatio);
    mVertices[2].setPoint(aAabb->upperBound.x * mRatio, aAabb->upperBound.y * mRatio);
    mVertices[3].setPoint(aAabb->lowerBound.x * mRatio, aAabb->upperBound.y * mRatio);
    Color4F color(aColor.r, aColor.g, aColor.b, 1);
    drawNode->drawPoly((Point *)mVertices, 8, true, color);
}
