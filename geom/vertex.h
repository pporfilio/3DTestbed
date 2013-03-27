#ifndef VERTEX_H
#define VERTEX_H

#include <vector>

#include "edge.h"
#include "polygon.h"
#include "material.h"
#include "QGLWidget"

class Edge;
class Polygon;

class Vertex
{
public:
    Vertex(float x, float y, float z);
    Vertex(vec3<float> v);

    Edge* getSharedEdge(const Vertex *other);

    void addPolygon(Polygon *p);
    void addEdge(Edge *e);
    float x() const;
    float y() const;
    float z() const;
    void glV3f() const;
    void print() const;
    void draw() const;
    vec3<float> asVec3() const;
    std::vector<Edge *> getEdges() const;
    void drawAnnotations(QGLWidget *widget) const;
    void pushMaterial(Material *mtl);
    void moveDelta(const vec3<float> delta);

private:
    float _x, _y, _z;
    std::vector<Edge *> edges;
    std::vector<Polygon *> polygons;
    Material *_mtl;
};

#endif // VERTEX_H
