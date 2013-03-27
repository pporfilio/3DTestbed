#ifndef AABB_H
#define AABB_H

#include <iostream>
#include <stdexcept>
#include <vector>
#include "vector.h"
#include <glu.h>
#include <QGLWidget>


#include "utils.h"

//simple 3D axis-aligned bounding box.

template <class T> class AABB {

private:
    T _minX;
    T _maxX;
    T _minY;
    T _maxY;
    T _minZ;
    T _maxZ;

public:
    AABB(T minX, T maxX, T minY, T maxY, T minZ, T maxZ) :
        _minX(minX), _maxX(maxX), _minY(minY),
        _maxY(maxY), _minZ(minZ), _maxZ(maxZ)
    {    }

    AABB() :
        _minX(0), _maxX(0), _minY(0),
        _maxY(0), _minZ(0), _maxZ(0)
    {    }

    std::vector<vec3<T> > getVertices() {
        std::vector<vec3<T> > result = std::vector<vec3<T> >();
        result.push_back(vec3<T>(_minX, _minY, _minZ));
        result.push_back(vec3<T>(_minX, _minY, _maxZ));
        result.push_back(vec3<T>(_minX, _maxY, _minZ));
        result.push_back(vec3<T>(_minX, _maxY, _maxZ));
        result.push_back(vec3<T>(_maxX, _minY, _minZ));
        result.push_back(vec3<T>(_maxX, _minY, _maxZ));
        result.push_back(vec3<T>(_maxX, _maxY, _minZ));
        result.push_back(vec3<T>(_maxX, _maxY, _maxZ));
        return result;
    }

    T maxX() {
        return _maxX;
    }

    T maxY() {
        return _maxY;
    }

    T maxZ() {
        return _maxZ;
    }

    T minX() {
        return _minX;
    }

    T minY() {
        return _minY;
    }

    T minZ() {
        return _minZ;
    }

    void setMaxY(T newMaxY) {
        _maxY = newMaxY;
    }

    void setMinY(T newMinY) {
        _minY = newMinY;
    }

    void setMaxX(T newMaxX) {
        _maxX = newMaxX;
    }

    void setMinX(T newMinX) {
        _minX = newMinX;
    }

    void setMaxZ(T newMaxZ) {
        _maxZ = newMaxZ;
    }

    void setMinZ(T newMinZ) {
        _minZ = newMinZ;
    }


    T getDepth() {
        return _maxZ - _minZ;
    }

    T getHeight() {
        return _maxY - _minY;
    }

    T getWidth() {
        return _maxX - _minX;
    }

    float halfWidth() {
        return (_maxX - _minX) / 2.0f;
    }

    float halfHeight() {
        return (_maxY - _minY) / 2.0f;
    }

    float halfDepth() {
        return (_maxZ - _minZ) / 2.0f;
    }


    void print() {
        std::cout << "MinX = " << _minX << " MaxX = " << _maxX << " MinY = "
             << _minY << " MaxY = " << _maxY << " MinZ = " << _minZ
             << " MaxZ = " << _maxZ << std::endl;
    }

    vec3<T> getCenter() {
        return vec3<T>((_maxX + _minX)/2.0f,
                       (_maxY + _minY)/2.0f,
                       (_maxZ + _minZ)/2.0f);
    }

    float getVolume() {
        return getWidth() * getHeight() * getDepth();
    }

    //note: this will behave strangely if T is an int
    void setPosition(vec3<T> newCenter) {
        vec3<T> oldCenter = getCenter();
        vec3<T> delta = newCenter - oldCenter;
        _maxX += delta.x;
        _minX += delta.x;
        _maxY += delta.y;
        _minY += delta.y;
        _maxZ += delta.z;
        _minZ += delta.z;
    }

    bool contains(vec3<T> point) {
        return point.x <= _maxX && point.x >= _minX &&
                point.y <= _maxY  && point.y >= _minY &&
                point.z <= _maxZ && point.z >= _minZ;
    }

    AABB<float> scaledCopy(float scale) {
        AABB<float> copy = AABB<float>(0, getWidth() * scale,
                                       0, getHeight() * scale,
                                       0, getDepth() * scale);
        copy.setPosition(getCenter() * scale);
        return copy;
    }

    //returns a vector of points that are evenly spaced along each dimension
    //within this bounding box. The parameters specify the number of points
    //along each dimension. A value of 1 will give a point in the center of that
    //dimension, 2 will give the endpoints, and k > 2 will give the endpoints
    //plus k - 2 points evenly spaced between (3 will give the endpoints and
    //center point, e.g.).
    std::vector<vec3<T> > pointArray(int nX, int nY, int nZ) {
        std::vector<float> xVals = linearArray(_minX, _maxX, nX);
        std::vector<float> yVals = linearArray(_minY, _maxY, nY);
        std::vector<float> zVals = linearArray(_minZ, _maxZ, nZ);

        std::vector<vec3<T> > result;
        typedef std::vector<float>::const_iterator iter;
        for (iter x = xVals.begin(); x < xVals.end(); ++x) {
            for (iter y = yVals.begin(); y < yVals.end(); ++y) {
                for (iter z = zVals.begin(); z < zVals.end(); ++z) {
                    result.push_back(vec3<T>(*x, *y, *z));
                }
            }
        }

        return result;
    }


    void drawOutline(float r = 0.0f, float g = 0.0f, float b = 0.0f, float lineWidth = 5.0) {
        glDisable(GL_TEXTURE_2D);
        glColor3f(r, g, b);
        glLineWidth(lineWidth);
        glBegin(GL_LINES);

        T xi = minX();
        T xa = maxX();
        T yi = minY();
        T ya = maxY();
        T zi = minZ();
        T za = maxZ();

        glVertex3f(xi, yi, za);
        glVertex3f(xa, yi, za);

        glVertex3f(xa, yi, za);
        glVertex3f(xa, ya, za);

        glVertex3f(xa, ya, za);
        glVertex3f(xi, ya, za);

        glVertex3f(xi, ya, za);
        glVertex3f(xi, yi, za);

        glVertex3f(xi, yi, za);
        glVertex3f(xi, yi, zi);

        glVertex3f(xi, ya, za);
        glVertex3f(xi, ya, zi);

        glVertex3f(xa, ya, za);
        glVertex3f(xa, ya, zi);

        glVertex3f(xa, yi, za);
        glVertex3f(xa, yi, zi);

        glVertex3f(xa, yi, zi);
        glVertex3f(xi, yi, zi);

        glVertex3f(xi, yi, zi);
        glVertex3f(xi, ya, zi);

        glVertex3f(xi, ya, zi);
        glVertex3f(xa, ya, zi);

        glVertex3f(xa, ya, zi);
        glVertex3f(xa, yi, zi);

        glEnd();
    }

    void drawSolid() {
        glColor3f(.8f, 0, 0);
        glBegin(GL_QUADS);

        //front
        glVertex3f(_minX, _minY, _maxZ);
        glVertex3f(_maxX, _minY, _maxZ);
        glVertex3f(_maxX, _maxY, _maxZ);
        glVertex3f(_minX, _maxY, _maxZ);

        //back
        glVertex3f(_minX, _minY, _minZ);
        glVertex3f(_minX, _maxY, _minZ);
        glVertex3f(_maxX, _maxY, _minZ);
        glVertex3f(_maxX, _minY, _minZ);

        //right
        glVertex3f(_maxX, _minY, _maxZ);
        glVertex3f(_maxX, _minY, _minZ);
        glVertex3f(_maxX, _maxY, _minZ);
        glVertex3f(_maxX, _maxY, _maxZ);

        //left
        glVertex3f(_minX, _minY, _maxZ);
        glVertex3f(_minX, _maxY, _maxZ);
        glVertex3f(_minX, _maxY, _minZ);
        glVertex3f(_minX, _minY, _minZ);

        //top
        glVertex3f(_minX, _maxY, _maxZ);
        glVertex3f(_maxX, _maxY, _maxZ);
        glVertex3f(_maxX, _maxY, _minZ);
        glVertex3f(_minX, _maxY, _minZ);

        //bottom
        glVertex3f(_minX, _minY, _maxZ);
        glVertex3f(_minX, _minY, _minZ);
        glVertex3f(_maxX, _minY, _minZ);
        glVertex3f(_maxX, _minY, _maxZ);

        glEnd();
    }


    //must bind texture first
    //each Vector2 is the min UV coordinates. texStep is the size of the (square) texture
    void drawTextured(Vector2 front, Vector2 back, Vector2 right,
                      Vector2 left, Vector2 top, Vector2 bottom, float texStep) {
        glBegin(GL_QUADS);

        //front
        glTexCoord2f(front.x + texStep, front.y + texStep);
        glVertex3f(_minX, _minY, _maxZ);
        glTexCoord2f(front.x, front.y + texStep);
        glVertex3f(_maxX, _minY, _maxZ);
        glTexCoord2f(front.x, front.y);
        glVertex3f(_maxX, _maxY, _maxZ);
        glTexCoord2f(front.x + texStep, front.y);
        glVertex3f(_minX, _maxY, _maxZ);

        //back
        glTexCoord2f(back.x + texStep, back.y + texStep);
        glVertex3f(_minX, _minY, _minZ);
        glTexCoord2f(back.x + texStep, back.y);
        glVertex3f(_minX, _maxY, _minZ);
        glTexCoord2f(back.x, back.y);
        glVertex3f(_maxX, _maxY, _minZ);
        glTexCoord2f(back.x, back.y + texStep);
        glVertex3f(_maxX, _minY, _minZ);

        //right
        glTexCoord2f(right.x + texStep, right.y + texStep);
        glVertex3f(_maxX, _minY, _maxZ);
        glTexCoord2f(right.x, right.y + texStep);
        glVertex3f(_maxX, _minY, _minZ);
        glTexCoord2f(right.x, right.y);
        glVertex3f(_maxX, _maxY, _minZ);
        glTexCoord2f(right.x + texStep, right.y);
        glVertex3f(_maxX, _maxY, _maxZ);

        //left
        glTexCoord2f(left.x + texStep, left.y + texStep);
        glVertex3f(_minX, _minY, _maxZ);
        glTexCoord2f(left.x + texStep, left.y);
        glVertex3f(_minX, _maxY, _maxZ);
        glTexCoord2f(left.x, left.y);
        glVertex3f(_minX, _maxY, _minZ);
        glTexCoord2f(left.x, left.y + texStep);
        glVertex3f(_minX, _minY, _minZ);

        //top
        glTexCoord2f(top.x + texStep, top.y);
        glVertex3f(_minX, _maxY, _maxZ);
        glTexCoord2f(top.x, top.y);
        glVertex3f(_maxX, _maxY, _maxZ);
        glTexCoord2f(top.x, top.y + texStep);
        glVertex3f(_maxX, _maxY, _minZ);
        glTexCoord2f(top.x + texStep, top.y + texStep);
        glVertex3f(_minX, _maxY, _minZ);

        //bottom
        glTexCoord2f(bottom.x, bottom.y);
        glVertex3f(_minX, _minY, _maxZ);
        glTexCoord2f(bottom.x, bottom.y + texStep);
        glVertex3f(_minX, _minY, _minZ);
        glTexCoord2f(bottom.x + texStep, bottom.y + texStep);
        glVertex3f(_maxX, _minY, _minZ);
        glTexCoord2f(bottom.x + texStep, bottom.y);
        glVertex3f(_maxX, _minY, _maxZ);

        glEnd();
    }

    bool intersects(AABB<T> other) {
        return other.minX() <= maxX() && minX() <= other.maxX()
                && other.minY() <= maxY() && minY() <= other.maxY()
                && other.minZ() <= maxZ() && minZ() <= other.maxZ();
    }


    void annotate(QGLWidget *widget) {
        QFont font("Deja Vu Sans Mono", 16, 4);
        std::vector<vec3<double> > verts = getVertices();
        typedef std::vector<vec3<double> >::const_iterator viter;
        for (viter v = verts.begin(); v < verts.end(); ++v) {
            glColor3f(1, 1, 1);
            widget->renderText(v->x, v->y, v->z, "(" + QString::number(v->x, 'g', 3) +
                       ", " + QString::number(v->y, 'g', 3) + ", " +
                               QString::number(v->z, 'g', 3) + ")", font);
        }
    }

    void expandToInclude(vec3<T> newPoint) {
        _minX = _minX < newPoint.x ? _minX : newPoint.x;
        _minY = _minY < newPoint.y ? _minY : newPoint.y;
        _minZ = _minZ < newPoint.z ? _minZ : newPoint.z;

        _maxX = _maxX > newPoint.x ? _maxX : newPoint.x;
        _maxY = _maxY > newPoint.y ? _maxY : newPoint.y;
        _maxZ = _maxZ > newPoint.z ? _maxZ : newPoint.z;
    }
};


#endif // AABB_H
