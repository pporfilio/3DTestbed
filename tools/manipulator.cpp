#include "manipulator.h"
#include "glu.h"

//Found a really cool document about interaction with objects in 3D:
// http://ironduke.cs.gsu.edu/gso_classes/private/siggraph_courses/S2002/disk1_1_23/notes/20/20.pdf

Manipulator::Manipulator() : _location(0, 0, 0), _size(1), _visible(false), _grabbed(false)
{
    Vertex *origin = new Vertex(_location.x, _location.y, _location.z);
    _xAxis = new Edge(origin, new Vertex(origin->asVec3() + vec3<float>(1, 0, 0)));
    _yAxis = new Edge(origin, new Vertex(origin->asVec3() + vec3<float>(0, 1, 0)));
    _zAxis = new Edge(origin, new Vertex(origin->asVec3() + vec3<float>(0, 0, 1)));

    allMaterials["__toolRed__"] = new Material(vec3<float>(1, 0, 0), 3);
    allMaterials["__toolGreen__"] = new Material(vec3<float>(0, 1, 0), 3);
    allMaterials["__toolBlue__"] = new Material(vec3<float>(0, 0, 1), 3);

    resetMaterials();

    _mesh = new Mesh();
    _mesh->addAllFromEdge(_xAxis);
    _mesh->addAllFromEdge(_yAxis);
    _mesh->addAllFromEdge(_zAxis);
}

Manipulator::~Manipulator() {
    _mesh->deleteContents();
}

void Manipulator::setAffectedVertices(std::set<Vertex *> vertices) {
    _affectedVerts.clear();

    if (vertices.size() == 0) {
        return;
    }

    vec3<float> sum(0, 0, 0);
    for (std::set<Vertex *>::const_iterator v = vertices.begin(); v != vertices.end(); ++v) {
        _affectedVerts.append((*v));
        sum += (*v)->asVec3();
    }
    vec3<float> delta = (sum  / vertices.size()) - _location;
    _location += delta;
    foreach(Vertex *v, _mesh->getVertices()) {
        v->moveDelta(delta);
    }

}

bool Manipulator::tryGrab(vec2<int> screenClickLoc) {

    float dist;
    Edge *nearestEdge = getNearestScreenspaceEdge(_mesh->getEdges(),
                                                  vec2<float>(screenClickLoc.x,
                                                              screenClickLoc.y),
                                                  dist);

    if (dist < 25) {
        nearestEdge->pushMaterial(allMaterials["edge_select"]);
        _transEdge = nearestEdge;
        _grabbed = true;
        return true;
    }
    return false;
}

bool Manipulator::isGrabbed() {
    return _grabbed;
}

void Manipulator::endMove() {
    _grabbed = false;
    resetMaterials();
}

void Manipulator::resetMaterials() {
    _xAxis->pushMaterial(allMaterials["__toolRed__"]);
    _yAxis->pushMaterial(allMaterials["__toolGreen__"]);
    _zAxis->pushMaterial(allMaterials["__toolBlue__"]);
}

void Manipulator::move(vec2<int> initScreenLoc, vec2<int> delta) {

    //for debugging
    _initScreenPoint = initScreenLoc;
    _endScreenPoint = initScreenLoc + delta;


    //get the point on the manipulator handle closest to the ray of the previous mouse
    //location going into the scene. This will be the point on the handle that is being moved.
    vec3<float> start3DLoc = nearestPointOnEdgeToClickRay(_transEdge, initScreenLoc);

    //get the point on the manip handle closest to where the user moved the mouse to.
    //This is where I want the initial point on the handle that was grabbed to end up.
    vec3<float> end3DLoc = nearestPointOnEdgeToClickRay(_transEdge, initScreenLoc + delta);//_transEdge->v1()->asVec3() + (v * t);

    //then the delta is the closest point to the end mouse ray minus the closest
    //point to the start mouse ray
    vec3<float> delta3D = end3DLoc - start3DLoc;

    //for debugging
    _debugStartPoint = start3DLoc;
    _debugEndPoint = end3DLoc;

    foreach (Vertex *v, _affectedVerts) {
        v->moveDelta(delta3D);
    }
    foreach (Vertex *v, _mesh->getVertices()) {
        v->moveDelta(delta3D);
    }
    _location += delta3D;
}

vec3<float> Manipulator::nearestPointOnEdgeToClickRay(Edge *edge, vec2<int> click) {
    //not great to do this each time this function is called
    //but I prefer it because it's cleaner than having to pass them in.
    //keeping them as member variables would require a bunch of bookkeeping.
    double mvmat[16];
    double projmat[16];
    int viewport[4];
    double dX, dY, dZ;

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mvmat);
    glGetDoublev(GL_PROJECTION_MATRIX, projmat);


    //v is the direction of the edge
    vec3<float> v = edge->v2()->asVec3() - edge->v1()->asVec3();

    //use the screen location of the mouse to get a ray going into the scene.
    gluUnProject(click.x, click.y, 0, mvmat, projmat, viewport, &dX, &dY, &dZ);
    vec3<float> rp1(dX, dY, dZ);
    gluUnProject(click.x, click.y, 1, mvmat, projmat, viewport, &dX, &dY, &dZ);
    vec3<float> rp2(dX, dY, dZ);

    //that ray's direction is u
    vec3<float> u = rp2 - rp1;

    //use this formula from http://geomalgorithms.com/a07-_distance.html
    // to get the distance t along the edge to the nearest point to the mouse ray
    vec3<float> w = rp1 - edge->v1()->asVec3();
    float a = u.dot(u);
    float b = u.dot(v);
    float c = v.dot(v);
    float d = u.dot(w);
    float e = v.dot(w);

    float t = (a * e - b * d)/(a * c - b * b);

    return edge->v1()->asVec3() + (v * t);


}


void Manipulator::draw(GLWidget *widget) {

    glDisable(GL_DEPTH_TEST);

    _mesh->draw(widget);

    glPointSize(5);
    glBegin(GL_POINTS);

    glColor3f(0, 0, 0);

    glVertex3f(_debugStartPoint.x,
               _debugStartPoint.y,
               _debugStartPoint.z);

    glColor3f(1, 1, 1);

    glVertex3f(_debugEndPoint.x,
               _debugEndPoint.y,
               _debugEndPoint.z);

    glEnd();

//    glColor3f(.8, .8, 1);
//    glLineWidth(3);
//    glBegin(GL_LINES);
//    glVertex3f(_mouseRay.first.x, _mouseRay.first.y, _mouseRay.first.z);
//    glVertex3f(_mouseRay.second.x, _mouseRay.second.y, _mouseRay.second.z);
//    glEnd();


//    glColor3f(1, .8, .8);
//    glPointSize(5);
//    push2D(widget); {
//        glBegin(GL_POINTS);
//        glVertex2d(_initScreenPoint.x, _initScreenPoint.y);
//    } pop2D();

    glEnable(GL_DEPTH_TEST);
}
