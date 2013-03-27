#include "glwidget.h"
#include <iostream>
#include <QGLFramebufferObject>
#include <QGLShaderProgram>
#include <QMouseEvent>
#include <QTime>
#include <QTimer>
#include <QWheelEvent>
#include <GLU.h>
#include "glm.h"
#include "utils.h"
#include <QElapsedTimer>
#include "CS123Algebra.h"
#include "geom/material.h"
#include "ray.h"
#include "aabb2D.h"

#include <sstream>


using std::cout;
using std::endl;

extern "C"
{
    extern void /*APIENTRY*/ glActiveTexture(GLenum);
}

static const int MAX_FPS = 120;

/**
  Constructor.  Initialize all member variables here.
 **/
GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent),
    m_show_text(false),
    m_mouseState(),
    m_timer(this),
    m_prevTime(0),
    m_prevFps(0.f),
    m_fps(0.f),
    m_selectMode(POLYGON),
    m_selectionCenter(0, 0, 0),
    m_currGeomModeIndex(2),
    m_font("Deja Vu Sans Mono", 16, 4),
    m_moveToolActive(false),
    m_manipSize(1),
    m_option_down(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    m_camera = m_orbitCamera = new OrbitCamera();
    bg_color = Vector4(1.0,1.0,1.0,1.0);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_prevTime = m_clock.elapsed();
    createMaterials();
    m_mesh = Mesh::CubeMesh(vec3<float>(0, 0, 0), 2);

    m_geomModes[0] = VERTEX;
    m_geomModes[1] = EDGE;
    m_geomModes[2] = POLYGON;
    m_geomModes[3] = OBJECT;

    m_manip = new Manipulator();

}

void GLWidget::createMaterials() {
    allMaterials = std::map<std::string, Material *>();
    DEFAULT_POLY_MTL = std::string("__default__polygon__material__");
    DEFAULT_EDGE_MTL = std::string("__default__edge__material__");
    DEFAULT_VERT_MTL = std::string("__default__vertex__material__");
    addMaterial(DEFAULT_POLY_MTL, new Material(vec3<float>(.6, .6, .6), 0.0f));
    addMaterial(DEFAULT_EDGE_MTL, new Material(vec3<float>(.45, .45, .45), 2.0f));
    addMaterial(DEFAULT_VERT_MTL, new Material(vec3<float>(.1, .1, .1), 4.0f));
    addMaterial(std::string("poly_select"), new Material(vec3<float>(.5, .8, .5), 0.0f));
    addMaterial(std::string("vert_select"), new Material(vec3<float>(.5, .8, .5), 4.0f));
    addMaterial(std::string("edge_select"), new Material(vec3<float>(.5, .8, .5), 2.0f));


}

/**
  Destructor.  Delete any 'new'ed objects here.
 **/
GLWidget::~GLWidget()
{
    foreach (QGLShaderProgram *sp, m_shaderPrograms)
        delete sp;

    delete m_camera;

    m_mesh->deleteContents();
}

/**
  Initialize the OpenGL state and start the drawing loop.
 **/
void GLWidget::initializeGL()
{
    glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);			// Nice Gray Background
    glClearDepth(1.0f);									// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// floatly Nice Perspective Calculations
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Start the drawing timer
    m_timer.start(1000.0f / MAX_FPS);
}


/**
  Draws the scene to a buffer which is rendered to the screen when this function exits.
 **/
void GLWidget::paintGL()
{

    fflush(stdout); //just to make sure things print at least once per frame

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the fps
    int time = m_clock.elapsed();
    m_fps = 1000.f / (time - m_prevTime);
    m_prevTime = time;


    //handle user input

    if (m_option_down && m_mouseState.isLeft() && m_mouseState.isDragging()) {
        m_camera->mouseMove(m_mouseState.getDelta());
    }

    applyPerspectiveCamera(this->width(), this->height());

    switch(m_selectMode) {
    case VERTEX: {
        m_manip->setAffectedVertices(m_selectedVerts);
        break;
    }
    case EDGE: {
        std::set<Vertex *> edgeVerts;
        foreach(Edge *e, m_selectedEdges) {
            edgeVerts.insert(e->v1());
            edgeVerts.insert(e->v2());
        }

        m_manip->setAffectedVertices(edgeVerts);
        break;
    }
    case POLYGON: {
        std::set<Vertex *> polyVerts;
        foreach(Polygon *p, m_selectedPolys) {
            foreach(Vertex *v, p->getVertices()) {
                polyVerts.insert(v);
            }
        }

        m_manip->setAffectedVertices(polyVerts);
        break;
    }
    case OBJECT: {

        break;
    }
    }

    if (!m_option_down && m_mouseState.hasEvent(MouseButton::LEFT) &&
            m_mouseState.pollEvent(MouseButton::LEFT).event() == MouseButton::PRESS) {
        //initial click. Check for manipulator grab.
        if (m_moveToolActive) {
            m_manip->tryGrab(vec2<int>(m_mouseState.getCurrPos().x,
                                       this->screenHeight() - m_mouseState.getCurrY()));
        }
    }
    if (!m_option_down && m_mouseState.isDragging()) {
        if (m_manip->isGrabbed()) {
            m_manip->move(vec2<int>(m_mouseState.getPrevPos().x,
                                    screenHeight() - m_mouseState.getPrevPos().y),
                          m_mouseState.getDelta() * vec2<int>(1, -1));
        }
    }


    if (m_mouseState.isRight()) {

        vec2<int> screenLoc = m_mouseState.getCurrPos();

        doSelect(screenLoc);
    }


    m_mesh->draw(this);

    if (m_moveToolActive) {
        m_manip->draw(this);
    }

    paintText();

    push2D(this); {
        glColor3f(1, 0, 0);
        glBegin(GL_POINTS); {
        foreach(vec2<float> v, _debugPoints) {
            glVertex2f(v.x, v.y);
        }
        } glEnd();
    } pop2D();

    paintUI();


    //clear unused mouse events
    while(m_mouseState.hasEvent()) {
        m_mouseState.pollEvent();
    }
}

/**
  Called to switch to a perspective OpenGL camera.

  @param width: the viewport width
  @param height: the viewport height
**/
void GLWidget::applyPerspectiveCamera(float width, float height)
{
      float ratio = ((float) width) / height;
      vec3<float> eye(m_camera->getPos());
      vec3<float> looking_at = eye + m_camera->getLook3();

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(m_camera->getFOVY(), ratio, m_camera->getNearClip(),
                     m_camera->getFarClip());

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      gluLookAt(eye.x, eye.y, eye.z, looking_at.x, looking_at.y, looking_at.z,
                m_camera->getUp3().x, m_camera->getUp3().y, m_camera->getUp3().z);

}



/**
  Called when the mouse is dragged.  Rotates the camera based on mouse movement.
**/
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    m_mouseState.setNewLocation(event->x(), event->y());
//    V2 pos(event->x(), event->y());
//    if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton)
//    {
//        V2 delta = pos - m_prevMousePos;
//        m_camera->mouseMove(delta);
//    }
//    m_prevMousePos = pos;
}

MouseButton::Which QtToMouseButton(Qt::MouseButton QtB) {
    MouseButton::Which b;
    if (QtB == Qt::LeftButton) {
        b = MouseButton::LEFT;
    } else if (QtB == Qt::RightButton) {
        b = MouseButton::RIGHT;
    } else {
        b = MouseButton::MIDDLE;
    }
    return b;
}

/**
  Record a mouse press position.
 **/
void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_mouseState.mousePressed(QtToMouseButton(event->button()),
                              event->x(), event->y());
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
    m_mouseState.mouseReleased(QtToMouseButton(event->button()),
                              event->x(), event->y());
    if (m_moveToolActive) {
        m_manip->endMove();
    }
}

/**
  Called when the mouse wheel is turned.  Zooms the camera in and out.
**/
void GLWidget::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical)
    {
        m_camera->mouseWheel(event->delta(), (event->modifiers() == Qt::ShiftModifier));
    }
}


/**
  Called when the GLWidget is resized.
 **/
void GLWidget::resizeGL(int width, int height)
{
    // Resize the viewport
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
}

void GLWidget::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Option || event->key() == 16777251) {
        m_option_down = false;
    }
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    switch(event->key())
    {
    case Qt::Key_R: {
        m_camera->reset();
        break;
    }
    case Qt::Key_T: {
        m_show_text = !m_show_text;
        break;
    }
    case Qt::Key_A: {
        typedef set<Polygon *>::const_iterator p_iter;
        typedef std::set<Vertex *>::const_iterator v_iter;
        typedef std::set<Edge *>::const_iterator e_iter;

        switch(m_selectMode) {
        case VERTEX: {
            for (v_iter v = m_selectedVerts.begin(); v != m_selectedVerts.end(); ++v) {
                (*v)->pushMaterial(allMaterials[DEFAULT_VERT_MTL]);
            }
            m_selectedVerts.clear();
            break;
        }
        case EDGE: {
            for (e_iter e = m_selectedEdges.begin(); e != m_selectedEdges.end(); ++e) {
                (*e)->pushMaterial(allMaterials[DEFAULT_EDGE_MTL]);
            }
            m_selectedEdges.clear();
            break;
        }
        case POLYGON: {
            for (p_iter p = m_selectedPolys.begin(); p != m_selectedPolys.end(); ++p) {
                (*p)->pushMaterial(allMaterials[DEFAULT_POLY_MTL]);
            }
            m_selectedPolys.clear();
            break;
        }
        case OBJECT: {

            break;
        }
        }
        break;
    }
    case Qt::Key_Option:
    case 16777251:
    {
        m_option_down = true;
        break;
    }
    case Qt::Key_Space: {
        m_currGeomModeIndex ++;
        m_currGeomModeIndex = m_currGeomModeIndex % N_GEOM_MODES;
        m_selectMode = m_geomModes[m_currGeomModeIndex];
        break;
    }
    case Qt::Key_W: {
        m_moveToolActive = true;
        break;
    }
    case Qt::Key_Q: {
        m_moveToolActive = false;
        break;
    }
    default: {
        printf("key was: %d\n", event->key());
    }
    }

}


void GLWidget::paintUI() {
    AABB2D<int> topBorder(0, m_width, m_height, m_height - 100);
    push2D(this); {
        topBorder.drawFilled(vec4<float>(0, 0, 0, .8f));

        glColor4f(1, 1, 1, 1);
        vec2<int> selectModeTextLoc(10, 80);
        switch(m_selectMode) {
        case VERTEX: {
            renderText(selectModeTextLoc.x, selectModeTextLoc.y, "Selection: Vertex", m_font);
            break;
        }
        case EDGE: {
            renderText(selectModeTextLoc.x, selectModeTextLoc.y, "Selection: Edge", m_font);
            break;
        }
        case POLYGON: {
            renderText(selectModeTextLoc.x, selectModeTextLoc.y, "Selection: Polygon", m_font);
            break;
        }
        case OBJECT: {
            renderText(selectModeTextLoc.x, selectModeTextLoc.y, "Selection: Object", m_font);
            break;
        }
        }

    } pop2D();
}



/**
  Draws text for the FPS and screenshot prompt
 **/
void GLWidget::paintText()
{

    if (! m_show_text) {
        return;
    }
    glColor3f(0.f, 0.f, 0.f);

    // Combine the previous and current framerate
    if (m_fps >= 0 && m_fps < 1000)
    {
       m_prevFps *= 0.95f;
       m_prevFps += m_fps * 0.05f;
    }

    // QGLWidget's renderText takes xy coordinates, a string, and a font
    renderText(10, 20, "FPS: " + QString::number((int) (m_prevFps)), m_font);
    renderText(10, 35, "P: Save screenshot", m_font);
    vec3<float> pos = m_camera->getPos();
    renderText(10, 50, "eye location : (" + QString::number((double)(pos.x)) +
               ", " + QString::number((double)(pos.y)) + ", " +
               QString::number((double)(pos.z)) + ")", m_font);
//    renderText(10, 65, "DEPTH = " + QString::number((double)(10 / max(pos.lengthSquared(), 1.0))), m_font);
    renderText(10, 65, "DEPTH = " + QString::number((double)(pos.getMagnitude())), m_font);
    renderText(10, 80, "step size = " + QString::number((double)(.05*pos.getMagnitude())), m_font);

    renderText(10, 95, "resolution = " + QString::number(this->width()) + " x " + QString::number(this->height()));


    //reset color to white
    glColor3f(1.f, 1.f, 1.f);


}

int GLWidget::screenWidth() {
    return m_width;
}

int GLWidget::screenHeight() {
    return m_height;
}



