#include "mesh.h"
#include <algorithm>

#include "../lib/utils.h"


//when I put this in utils.h, I get linking errors
//note that other functions in utils, e.g. test(), do work.
template <class T> void addIfNotPresent(std::vector<T>& vec, T& element) {
    if (vec.end() == find(vec.begin(), vec.end(), element)) {
        vec.push_back(element);
    }
}

Mesh::Mesh()
{
}

//TODO: O(n) duplicate avoidance is unacceptable.
//Come up with better way to do this.
void Mesh::addPolygon( Polygon *p) {
    addIfNotPresent<Polygon *>(polygons, p);
}

void Mesh::addEdge( Edge *e) {
    fflush(stdout);
    addIfNotPresent(edges, e);
}

void Mesh::addVertex( Vertex *v) {
    addIfNotPresent(vertices, v);
}

void Mesh::addAllFromEdge(Edge *e) {
    addEdge(e);
    addVertex(e->v1());
    addVertex(e->v2());
}

void Mesh::addAllFromPolygon( Polygon *p) {
    addPolygon(p);
    typedef std::vector<Vertex *>::const_iterator v_iter;
    typedef std::vector<Edge *>::const_iterator e_iter;
    typedef std::vector<Vertex *>::size_type v_size;
    typedef std::vector<Edge *>::size_type e_size;

    //can't call getEdges() in loop and use iterators, because each time
    //a new copy of the list is returned, so incrementing an iterator on
    //one returned vector won't be applicable to the next returned vector.
    std::vector<Edge *> eToAdd = p->getEdges();

    for (e_iter e = eToAdd.begin(); e != eToAdd.end(); ++e) {
        addEdge(*e);
    }

    std::vector<Vertex *> vToAdd = p->getVertices();
    for (v_iter v = vToAdd.begin(); v < vToAdd.end(); ++v) {
        addVertex(*v);
    }

}

Mesh* Mesh::CubeMesh(const vec3<float> center, const float width) {
    Mesh *m = new Mesh();
    float hw = width / 2.0;

    float front = center.z + hw;
    float back = center.z - hw;
    float left = center.x - hw;
    float right = center.x + hw;
    float bottom = center.y - hw;
    float top = center.y + hw;

    Vertex *fll = new Vertex(front, bottom, left);
    Vertex *flr = new Vertex(front, bottom, right);
    Vertex *ful = new Vertex(front, top, left);
    Vertex *fur = new Vertex(front, top, right);
    Vertex *bll = new Vertex(back, bottom, left);
    Vertex *blr = new Vertex(back, bottom, right);
    Vertex *bul = new Vertex(back, top, left);
    Vertex *bur = new Vertex(back, top, right);

    //front
    m->addAllFromPolygon(new Polygon(4, fll, ful, fur, flr));

    //left
    m->addAllFromPolygon(new Polygon(4, ful, fll, bll, bul));

    //back
    m->addAllFromPolygon(new Polygon(4, blr, bur, bul, bll));

    //right
    m->addAllFromPolygon(new Polygon(4, flr, fur, bur, blr));

    //top
    m->addAllFromPolygon(new Polygon(4, fur, ful, bul, bur));

    //bottom
    m->addAllFromPolygon(new Polygon(4, fll, flr, blr, bll));

    return m;
}

void Mesh::draw(QGLWidget *widget) const {
    typedef std::vector<Polygon *>::const_iterator p_iter;
    typedef std::vector<Edge *>::const_iterator e_iter;
    typedef std::vector<Vertex *>::const_iterator v_iter;

    for (p_iter p = polygons.begin(); p < polygons.end(); ++p) {
        (*p)->draw();
    }

    for (e_iter e = edges.begin(); e < edges.end(); ++e) {
        (*e)->draw();
    }

    for (v_iter v = vertices.begin(); v < vertices.end(); ++v) {
        (*v)->draw();
        (*v)->drawAnnotations(widget);
    }
}

std::vector<Vertex*> Mesh::getVertices() {
    return vertices;
}

std::vector<Edge*> Mesh::getEdges() {
    return edges;
}

std::vector<Polygon*> Mesh::getPolygons() {
    return polygons;
}

Mesh::Intersection Mesh::getIntersectingPoly(const Ray r) const {

    typedef std::vector<Polygon *>::const_iterator p_iter;

    //for each polygon in the mesh
    float minT = INFINITY;
    Intersection intersection(INFINITY, NULL);
    for (p_iter p = polygons.begin(); p < polygons.end(); ++p) {
        float t = (*p)->intersect(r);
        if (t > 0 && t < minT) {
            intersection = Intersection(t, (*p));
        }
    }

    return intersection;
}

//convenience method to destroy all Vertices, Edges, and Polygons in this mesh.
//use carefully because these objects are created outside the mesh, so you likely
//have other references to them which will now be invalid.
void Mesh::deleteContents() {
    foreach(Polygon *p, polygons) {
        delete p;
    }

    foreach(Edge *e, edges) {
        delete e;
    }

    foreach(Vertex *v, vertices) {
        delete v;
    }
}
