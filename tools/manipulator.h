#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include <QList>
#include "vertex.h"
#include "mesh.h"
#include "glwidget.h"

class GLWidget;

class Manipulator
{
public:
    Manipulator();
    ~Manipulator();

    void setAffectedVertices(std::set<Vertex *> vertices);

    bool tryGrab(vec2<int> screenClickLoc);
    bool isGrabbed();
    void endMove();
    void resetMaterials();
    void move(vec2<int> initScreenLoc, vec2<int> delta);
    void draw(GLWidget *widget);

protected:
    vec3<float> nearestPointOnEdgeToClickRay(Edge *edge, vec2<int> click);

private:
    Mesh *_mesh;
    Edge *_xAxis;
    Edge *_yAxis;
    Edge *_zAxis;
    vec3<float> _location;
    Edge *_transEdge;
    float _size;
    QList<Vertex *> _affectedVerts;
    bool _visible;
    bool _grabbed;
    vec3<float> _debugStartPoint;
    vec3<float> _debugEndPoint;
    QPair<vec3<float>, vec3<float> > _mouseRay;
    vec2<int> _initScreenPoint;
    vec2<int> _endScreenPoint;
};

#endif // MANIPULATOR_H
