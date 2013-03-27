#ifndef AABB2D_H
#define AABB2D_H

#include "CS123Algebra.h"

//Simple 2D axis-aligned bounding box. Useful for 2D drawing, for example.

template <class T> class AABB2D {
private:
    T _left;
    T _right;
    T _top;
    T _bottom;

public:
    AABB2D(T left, T right, T top, T bottom) :
        _left(left), _right(right), _top(top), _bottom(bottom)
    {    }

    AABB2D() : _left(0), _right(0), _top(0), _bottom(0)
    {    }

    std::vector<vec2<T> > getVertices() {
        std::vector<vec2<T> > result;
        result.push_back(vec2<T>(_left, _top));
        result.push_back(vec2<T>(_left, _bottom));
        result.push_back(vec2<T>(_right, _bottom));
        result.push_back(vec2<T>(_right, _top));
        return result;
    }

    T left() {
        return _left;
    }

    T right() {
        return _right;
    }

    T top() {
        return _top;
    }

    T bottom() {
        return _bottom;
    }

    void setTop(T newTop) {
        _top = newTop;
    }

    void setBottom(T newBottom) {
        _bottom = newBottom;
    }

    void setLeft(T newLeft) {
        _left = newLeft;
    }

    void setRight(T newRight) {
        _right = newRight;
    }

    T getWidth() {
        return _right - _left;
    }

    T getHeight() {
        return _top - _bottom;
    }

    float halfHeight() {
        return getHeight() / 2.0f;
    }

    float halfWidth() {
        return getWidth() / 2.0f;
    }

    void print() {
        std::cout << "left = " << _left << " right = " << _right <<
                     " top = " << _top << " bottom = " << _bottom << std::endl;
    }

    vec2<float> getCenter() {
        return vec2<float>((_left + _right)/2.0f, (_top - _bottom)/2.0f);
    }

    float getArea() {
        return getWidth() * getHeight();
    }

    void setPosition(vec2<T> newCenter) {
        vec2<T> oldCenter = getCenter();
        vec2<T> delta = newCenter - oldCenter;
        _left += delta.x;
        _right += delta.x;
        _top += delta.y;
        _bottom += delta.y;
    }

    bool contains(vec2<T> point) {
        return point.x <= _right && point.x >= _left &&
                point.y <= _top && point.y >= _bottom;
    }

    AABB2D<float> scaledCopy(float scale) {
        AABB2D<float> copy = AABB2D<float>(0, getWidth() * scale,
                                         0, getHeight() * scale);
        copy.setPosition(getCenter() * scale);
        return copy;
    }

    //Highly recommended that you set up glOrtho, e.g. by
    //using push2D, before calling this.
    void drawOutline(vec4<float> color, float lineWidth=5.0) {
        glDisable(GL_TEXTURE_2D);
        glColor4f(color.x, color.y, color.z, color.w);
        glLineWidth(lineWidth);
        glBegin(GL_LINES);

        glVertex2f(_left, _top);
        glVertex2f(_right, _top);

        glVertex2f(_right, _top);
        glVertex2f(_right, _bottom);

        glVertex2f(_right, _bottom);
        glVertex2f(_left, _bottom);

        glVertex2f(_left, _bottom);
        glVertex2f(_left, _top);

        glEnd();
    }

    void drawFilled(vec4<float> color) {
        glDisable(GL_TEXTURE_2D);
        glColor4f(color.x, color.y, color.z, color.w);
        glBegin(GL_QUADS);

        glVertex2f(_left, _top);
        glVertex2f(_left, _bottom);
        glVertex2f(_right, _bottom);
        glVertex2f(_right, _top);

        glEnd();
    }

    void intersects(AABB2D<T> other) {
        return other.left() <= _right && _left <= other.right()
                && other.bottom() <= _top && _bottom <= other.top();
    }

};

#endif // AABB2D_H
