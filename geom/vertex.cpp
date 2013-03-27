#include "vertex.h"
#include <GLU.h>
#include "glm.h"


Vertex::Vertex(float x, float y, float z): _x(x), _y(y), _z(z)
{
    _mtl = allMaterials[DEFAULT_VERT_MTL];
}

Vertex::Vertex(vec3<float> v): _x(v.x), _y(v.y), _z(v.z) {
    _mtl = allMaterials[DEFAULT_VERT_MTL];
}


//This should probably not be n^2...but it might not matter because vertices
//shouldn't have *that* many edges in a geometric model...
Edge* Vertex::getSharedEdge(const Vertex *other) {
    foreach (Edge *e1, edges) {
        foreach (Edge *e2, other->getEdges()) {
            if (e1 == e2) {
                return e1;
            }
        }
    }
    return NULL;
}


std::vector<Edge *> Vertex::getEdges() const {
    return edges;
}

void Vertex::addEdge(Edge *e) {
    edges.push_back(e);
}

void Vertex::addPolygon(Polygon *p) {
    polygons.push_back(p);
}

float Vertex::x() const {
    return _x;
}

float Vertex::y() const {
    return _y;
}

float Vertex::z() const {
    return _z;
}

void Vertex::glV3f() const {
    glVertex3f(_x, _y, _z);
}

void Vertex::print() const {
    printf("(%.2f, %.2f, %.2f)", _x, _y, _z);
}

vec3<float> Vertex::asVec3() const {
    return vec3<float>(_x, _y, _z);
}

void Vertex::draw() const {
    _mtl->applyColor();
    glPointSize(_mtl->getLineWidth());
    glBegin(GL_POINTS);
    this->glV3f();
    glEnd();
}

void Vertex::drawAnnotations(QGLWidget *widget) const {
    _mtl->applyFontColor();
    glDisable(GL_DEPTH_TEST); {
        widget->renderText(_x, _y, _z,
                           "(" + QString::number(_x, 'g', 3) +
                           ", " + QString::number(_y, 'g', 3) +
                           ", " + QString::number(_z, 'g', 3) + ")",
                           _mtl->getFont());
    } glEnable(GL_DEPTH_TEST);
}

void Vertex::pushMaterial(Material *mtl) {
    _mtl = mtl;
}

void Vertex::moveDelta(const vec3<float> delta) {
    _x += delta.x;
    _y += delta.y;
    _z += delta.z;
}
