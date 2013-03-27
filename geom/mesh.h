#ifndef MESH_H
#define MESH_H

#include "polygon.h"
#include "edge.h"
#include "vertex.h"
#include "CS123Algebra.h"
#include "ray.h"

/*
 * A mesh is a collection of Vertices, Polygons, and Edges. Frequently these would
 * be interconnected but this could also be used as a container for a bunch of
 * unrelated geometry. I do have duplicate-checking, though, so the same vertex won't get
 * added twice.
 *
 * Generally you should use the addAllFromEdge and addAllFromPoly methods so that vertices and edges
 * also get added into the mesh lists.
 *
 */

class Mesh
{
public:
    struct Intersection {
        float t;
        Polygon *p;

        Intersection(float t, Polygon *p) : t(t), p(p) { }
    };

    Mesh();
    static Mesh* CubeMesh(const vec3<float> center, const float width);
    void addPolygon( Polygon *p);
    void addEdge( Edge *e);
    void addVertex( Vertex *v);
    void addAllFromEdge(Edge *e);
    void addAllFromPolygon( Polygon *p);
    void draw(QGLWidget *widget) const;
    Mesh::Intersection getIntersectingPoly(const Ray r) const;
    std::vector<Vertex *> getVertices();
    std::vector<Edge *> getEdges();
    std::vector<Polygon *> getPolygons();
    void deleteContents();
private:
    std::vector<Polygon *> polygons;
    std::vector<Edge *> edges;
    std::vector<Vertex *> vertices;
};

#endif // MESH_H
