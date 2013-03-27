#include "triangle.h"

Triangle::Triangle(vec3<float> v1, vec3<float> v2, vec3<float> v3) :
    v1(v1), v2(v2), v3(v3)
{
    normal = (v2 - v1).cross(v3 - v1);
}

vec3<float> Triangle::getNormal() const {
    return normal;
}


float Triangle::intersect(const Ray r) const {
    float t = (v1 - r.origin()).dot(normal) / r.direction().dot(normal);
    if (!pointInTriangle(r.pointAlong(t))) {
        return INFINITY;
    } else {
        return t;
    }
}

bool Triangle::pointInTriangle(const vec3<float> point) const {
    vec3<float> u = v2 - v1;
    vec3<float> v = v3 - v1;
    vec3<float> w = point - v1;

    vec3<float> nxv = normal.cross(v);
    vec3<float> nxu = normal.cross(u);

    float s = w.dot(nxv) / u.dot(nxv);
    float t = w.dot(nxu) / v.dot(nxu);

    return s >= 0 && t >= 0 && s + t <= 1;

}
