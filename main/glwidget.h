#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtGui/QMainWindow>
#include <QGLWidget>
#include <QHash>
#include <QString>
#include <QTimer>
#include <QTime>
#include <set>

#include "mainwindow.h"
#include "camera.h"
#include "OrbitingCamera.h"
#include "CS123Matrix.h"
#include "CS123Vector.h"
#include "vector.h"
#include "mouseinput.h"
#include "geom/material.h"
#include "geom/mesh.h"
#include "geom/polygon.h"
#include "geom/edge.h"
#include "geom/vertex.h"
#include "tools/manipulator.h"

class QGLShaderProgram;
class QGLFramebufferObject;
class Manipulator;

//I don't like this but the best alternative I found was elegant but
//a lot of code and felt like overkill. I don't expect to be adding GeomModes
//very often.
//but it's a pretty cool idea:
// http://stackoverflow.com/questions/1390703/enumerate-over-an-enum-in-c
#define N_GEOM_MODES 4

class GLWidget : public QGLWidget
{
    Q_OBJECT
    //Allow MainWindow to access private/protected members
    //Added to allow MainWindow::keyPressEvent to call GLWidget::keyPressEvent
    friend class MainWindow;

    enum GeomMode { VERTEX, EDGE, POLYGON, OBJECT };


public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    //various state
    Camera *m_orbitCamera;
    Camera *m_camera;

    Vector4 bg_color;

    bool m_show_text;

    MouseState m_mouseState;

    int screenWidth();
    int screenHeight();



protected:
    // Overridden QGLWidget methods
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyReleaseEvent(QKeyEvent *event);


    // Drawing code
    void applyPerspectiveCamera(float width, float height);
    void applyOrthogonalCamera(float width, float height);

    void paintUI();
    void paintText();

    void drawCube(Vector3 center, float size);
    void createMaterials();

    //implemented in geominteraction.cpp
    void doSelect(vec2<int> screenLoc);
    vec3<float> getSelectionCenter();
    void drawMoveManip();

private:
    QTimer m_timer;
    QTime m_clock;
    int m_prevTime;
    float m_prevFps, m_fps;
    GeomMode m_selectMode;
    vec3<float> m_selectionCenter;

    //TODO: ? Replace with enum iterator pattern:
    // http://stackoverflow.com/questions/1390703/enumerate-over-an-enum-in-c
    GeomMode m_geomModes[N_GEOM_MODES];
    int m_currGeomModeIndex;



    // Resources
    QHash<QString, QGLShaderProgram *> m_shaderPrograms; // hash map of all shader programs
    QHash<QString, QGLFramebufferObject *> m_framebufferObjects; // hash map of all framebuffer objects
    QFont m_font; // font for rendering text

    Mesh *m_mesh;
    //TODO: change to C++11 and use unordered_set
    std::set<Polygon *> m_selectedPolys;
    std::set<Vertex *> m_selectedVerts;
    std::set<Edge *> m_selectedEdges;

    int m_width;
    int m_height;

    QList<vec2<float> > _debugPoints;

    bool m_moveToolActive;
    float m_manipSize;

    bool m_option_down;
    Manipulator *m_manip;

};

#endif // GLWIDGET_H
