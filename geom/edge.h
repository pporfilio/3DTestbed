#ifndef EDGE_H
#define EDGE_H

#include <vector>

#include "vertex.h"
#include "polygon.h"
#include "material.h"

class Vertex;
class Polygon;

class Edge
{

public:
    Edge(Vertex *v1, Vertex *v2);
    Vertex* v1();
    Vertex* v2();
    bool contains(const Vertex *v) const;

    void addPolygon(Polygon *p);

    void draw();
    void print();
    void pushMaterial(Material *mtl);


private:
    std::vector<Polygon *> polygons;
    Vertex *_v1;
    Vertex *_v2;
    Material *_mtl;
};

#endif // EDGE_H
