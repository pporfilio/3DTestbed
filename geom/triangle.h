#ifndef TRIANGLE_H
#define TRIANGLE_H

/*
 * For now, Triangle doesn't use Vertex or Edge or anything because
 * I want to be able to create and destroy these easily and I don't
 * want it spawning a bunch of Edges that I have to clean up.
 *
 * If you want to use Edges and Vertices, use a Polygon instead of a Triangle
 */


#include "ray.h"

class Triangle
{
public:
    Triangle(vec3<float> v1, vec3<float> v2, vec3<float> v3);

    float intersect(const Ray r) const;
    bool pointInTriangle(const vec3<float> point) const;
    vec3<float> getNormal() const;

    vec3<float> v1;
    vec3<float> v2;
    vec3<float> v3;

    vec3<float> normal;
};

#endif // TRIANGLE_H
