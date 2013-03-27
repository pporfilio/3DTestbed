#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include "CS123Algebra.h"

#include "edge.h"
#include "vertex.h"
#include "material.h"
#include "ray.h"
#include "triangle.h"

class Vertex;
class Edge;

class Polygon
{
public:
    Polygon(int n, Vertex *v1, Vertex *v2, Vertex *v3, ...);

    //returns a copy of the vector of pointers. This way, callers can change
    //the vertices (i.e. their location) but not their order
    std::vector<Vertex *> getVertices() const;
    std::vector<Edge *> getEdges() const;
    float intersect(const Ray r) const;

    void pushMaterial(Material *mtl);
    void draw();
private:
    std::vector<Triangle> getTempTriangulation() const;
    std::vector<Vertex *> vertices;
    std::vector<Edge *> edges;
    Material *_mtl;
};

#endif // POLYGON_H
