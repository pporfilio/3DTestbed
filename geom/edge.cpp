#include "edge.h"
#include <GLU.h>
#include "glm.h"

Edge::Edge(Vertex *v1, Vertex *v2)
{
    _v1 = v1;
    _v2 = v2;
    _v1->addEdge(this);
    _v2->addEdge(this);
    _mtl = allMaterials[DEFAULT_EDGE_MTL];
}

bool Edge::contains(const Vertex *v) const {
    return v == _v1 || v == _v2;
}

Vertex* Edge::v1() {
    return _v1;
}

Vertex* Edge::v2() {
    return _v2;
}

void Edge::addPolygon(Polygon *p) {
    polygons.push_back(p);
}

void Edge::draw() {
    _mtl->applyColor();
    glLineWidth(_mtl->getLineWidth());
    glBegin(GL_LINES);
    _v1->glV3f();
    _v2->glV3f();
    glEnd();
}

void Edge::print() {
    printf("v1: ");
    if (_v1) {
        printf("%p", _v1);
    } else {
        printf("NULL");
    }
    printf(" v2: ");
    if (_v2) {
        printf("%p", _v2);
    } else {
        printf("NULL");
    }
    printf("\n");
}

void Edge::pushMaterial(Material *mtl) {
    _mtl = mtl;
}
