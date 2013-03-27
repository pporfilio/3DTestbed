#include "ray.h"
#include "utils.h"
#include <glu.h>
#include <stdexcept>

Ray::Ray(const vec3<float> origin, const vec3<float> direction): _origin(origin)
{
    if (direction.isZero()) {
        throw std::domain_error("direction of ray may not be 0");
    }
    _direction = direction.getNormalized();
}

vec3<float> Ray::pointAlong(float t) const {
    return _origin + (t * _direction);
}

vec3<float> Ray::direction() const {
    return _direction;
}

vec3<float> Ray::origin() const {
    return _origin;
}

void Ray::bump(float distance) {
    _origin += _direction * distance;
}

void Ray::print() const {
    printf("Ray origin = ");
    printVec3(_origin);
    printf("  direction = ");
    printVec3(_direction);
    printf("\n");
}

void Ray::draw(float r, float g, float b, float length) const {
    glColor3f(r, g, b);
    glLineWidth(4.0f);
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_LINES);
    glVertex3f(_origin.x, _origin.y, _origin.z);
    vec3<float> end = _origin + (_direction * length);
    glVertex3f(end.x, end.y, end.z);
    glEnd();
    glEnable(GL_DEPTH_TEST);
}
