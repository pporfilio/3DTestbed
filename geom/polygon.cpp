#include "polygon.h"
#include <GLU.h>
#include "glm.h"
#include "utils.h"

//ensure polygon has at least 3 vertices
//n is the total number of vertices
Polygon::Polygon(int n, Vertex *v1, Vertex *v2, Vertex *v3, ...) {

    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);

    va_list vargs;
    va_start(vargs, v3);

    for (int i = 0; i < n - 3; i++) {
        Vertex *tmp = va_arg(vargs, Vertex*);
        vertices.push_back(tmp);
    }

    va_end(vargs);

    typedef std::vector<Vertex *>::const_iterator iter;
    iter v1_i = vertices.begin();
    iter v2_i = ++vertices.begin();
    Edge *e;

    //for each pair of vertices
    for (; v1_i != vertices.end(); ++v1_i, ++v2_i) {
        //vertices make a loop, so reset second vertex to beginning
        //when v2 has advanced past the last vertex
        if (v2_i == vertices.end()) {
            v2_i = vertices.begin();
        }

        //try to get a shared edge between these vertices
        e = (*v1_i)->getSharedEdge(*v2_i);
        //if none exists,
        if (!e) {
            //create a new edge and add it to edges
            e = new Edge(*v1_i, *v2_i);
        }

        //add the edge between these verts to our list
        edges.push_back(e);
        //add this poly to that edge's list
        e->addPolygon(this);
        //add this poly to v1
        v1->addPolygon(this);
    }

    _mtl = allMaterials.at(DEFAULT_POLY_MTL);
}

std::vector<Vertex *> Polygon::getVertices() const {
    return vertices;
}

std::vector<Edge *> Polygon::getEdges() const {
    return edges;
}

std::vector<Triangle> Polygon::getTempTriangulation() const {
    std::vector<Triangle> tris;
    for (std::vector<Triangle>::size_type i = 1; i < vertices.size() - 1; i++) {
        tris.push_back(Triangle(vertices.at(0)->asVec3(),
                                vertices.at(i)->asVec3(),
                                vertices.at(i+1)->asVec3()));
    }
    return tris;
}

float Polygon::intersect(const Ray r) const {
    std::vector<Triangle> tris = getTempTriangulation();
    typedef std::vector<Triangle>::const_iterator t_iter;

    float minT = INFINITY;
    for (t_iter t = tris.begin(); t < tris.end(); ++t) {
        minT = std::min(minT, (*t).intersect(r));
    }
    return minT;

}

void Polygon::draw() {
    _mtl->applyColor();
    glBegin(GL_POLYGON);
    typedef std::vector<Vertex *>::const_iterator v_iter;
    for (v_iter v = vertices.begin(); v < vertices.end(); ++v) {
        (*v)->glV3f();
    }
    glEnd();

}

void Polygon::pushMaterial(Material *mtl) {
    _mtl = mtl;
}


