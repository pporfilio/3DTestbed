#ifndef RAY_H
#define RAY_H

#include "CS123Algebra.h"

class Ray
{
public:
    //A ray going in the direction point1 to point2.
    Ray(const vec3<float> origin, const vec3<float> direction);
    vec3<float> origin() const;
    vec3<float> direction() const;
    void bump(float distance);
    void draw(float r, float g, float b, float length = 1.0f) const;
    void print() const;
    vec3<float> pointAlong(float t) const;


private:
    vec3<float> _origin;
    vec3<float> _direction;
};

#endif // RAY_H
