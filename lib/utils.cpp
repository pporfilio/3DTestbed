#include "utils.h"
#include <QFileDialog>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <qgl.h>
#include <glu.h>

float minN(int n, ...) {
    double tmp;
    double min;

    va_list v1;
    va_start(v1, n);

    min = va_arg(v1, double);
    for (int i = 1; i < n; i++) {
        tmp = va_arg(v1, double);
        min = tmp < min ? tmp : min;
    }

    va_end(v1);

    return (float)min;
}

float maxN(int n, ...) {
    double tmp;
    double max;

    va_list v1;
    va_start(v1, n);

    max = va_arg(v1, double);
    for (int i = 1; i < n; i++) {
        tmp = va_arg(v1, double);
        max = tmp > max ? tmp : max;
    }

    va_end(v1);

    return (float)max;
}



double toRadians(double degrees) {
    return (degrees / 180.0) * PI;
}

void printMat4x4(const Matrix4x4 &m) {
    for (int i = 0; i < 4; i++) {
        printf("\n[");
        for (int j = 0; j < 4; j++) {
            printf(" %.2f", m.data[i*4 + j]);
        }
        printf(" ]");
    }
    printf("\n\n");
}

void printVec4(const Vector4 &v, bool newline) {
    printf("(%.2f, %.2f, %.2f, %.2f)", v.x, v.y, v.z, v.w);
    if (newline) {
        printf("\n");
    }
}

void printVec3(const vec3<float> &v, bool newline) {
    printf("(%.2f, %.2f, %.2f)", v.x, v.y, v.z);
    if (newline) {
        printf("\n");
    }
}


/* dst_arr must be large enough to accept all the characters in qstr
    Fills dst_arr with qstr's character data.
*/
void QStringToChar(QString *qstr, char *dst_arr) {
    QByteArray ba = qstr->toLocal8Bit();
    const char *c_str = ba.data();
    int len = ba.size() + 1; //+1 for null terminating character
    for (int i = 0; i < len; i++) {
        dst_arr[i] = c_str[i];
    }
}


//given a center, a radius, and a number n, return n evenly spaced points
//in the X-Z plane at height Y.
std::vector<vec3<REAL> > getPointsOnCircle(vec3<REAL> center, double radius, int n) {
    std::vector<vec3<REAL> > points;
    for (int i = 0; i < n; i++) {
        double theta = 2 * i * PI / n;
        points.push_back(vec3<REAL>(center.x + radius * cos(theta),
                            center.y,
                            center.z + radius * sin(theta)));
    }
    return points;
}



std::vector<vec3<REAL> > getPointsOnSphere(vec3<REAL> center, double radius,
                                    int n_sides, int n_vertical_divisions) {
    vec3<REAL> top = vec3<REAL>(center.x, center.y + radius, center.z);
    vec3<REAL> bottom = vec3<REAL>(center.x, center.y - radius, center.z);

    std::vector<vec3<REAL> > points;
    points.push_back(top);
    points.push_back(bottom);

    std::vector<double> radii;
    std::vector<vec3<REAL> > heights;

    double theta = PI / n_vertical_divisions;

    for (int i = 1; i < n_vertical_divisions; i++) {
        radii.push_back(sin(theta*i)*radius);
        heights.push_back(vec3<REAL>(center.x,
                               center.y + cos(theta*i)*radius,
                               center.z));
        //same radius but negative height
        radii.push_back(sin(theta*i)*radius);
        heights.push_back(vec3<REAL>(center.x,
                               center.y - cos(theta*i)*radius,
                               center.z));
    }

    int len = heights.size();
    for (int i = 1; i < len; i++) {
        std::vector<vec3<REAL> > points_around = getPointsOnCircle(heights[i], radii[i], n_sides);
        int size = points_around.size();
        for (int i = 0; i < size; i++) {
            points.push_back(points_around[i]);
        }
    }
    return points;
}


//Prompts user to select a file or directory using QFileDialog.
//Will present a save dialog if save is true, else presents an open dialog.
QString myGetSaveFileName(QWidget *parent, const QString &caption,
                                    const QString &dir, const QString &filter,
                                    bool save) {

    QString selectedFilter;
    QString selected_path;

    if (save) {
        selected_path = QFileDialog::getSaveFileName(parent, caption,
                                                     dir, filter, &selectedFilter);
    } else {
        selected_path = QFileDialog::getOpenFileName(parent, caption,
                                                     dir, filter, &selectedFilter);
    }

    return selected_path;
}


vec3<float> centroid(const std::vector<Vertex *> points) {
    vec3<float> sum;
    typedef std::vector<Vertex *>::const_iterator v_iter;
    for (v_iter v = points.begin(); v != points.end(); ++v) {
        sum += (*v)->asVec3();
    }
    sum /= points.size();
    return sum;
}

void push2D(GLWidget *widget) {
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, widget->screenWidth(), 0, widget->screenHeight(), 50, -50);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

void pop2D() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

Edge *getNearestScreenspaceEdge(std::vector<Edge *> edges, vec2<float> loc, float &dist) {
    typedef std::vector<Edge *>::const_iterator e_iter;

    double mvmat[16];
    double projmat[16];
    int viewport[4];
    double dX, dY, dZ;

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mvmat);
    glGetDoublev(GL_PROJECTION_MATRIX, projmat);

    Edge *nearestEdge = NULL;
    dist = INFINITY;
    for (e_iter e = edges.begin(); e < edges.end(); ++e) {
        vec3<float> v1 = (*e)->v1()->asVec3();
        vec3<float> v2 = (*e)->v2()->asVec3();

        // screenspace edge

        gluProject(v1.x, v1.y, v1.z, mvmat, projmat, viewport, &dX, &dY, &dZ);
        vec2<float> sv1 = vec2<float>(dX, dY);
        gluProject(v2.x, v2.y, v2.z, mvmat, projmat, viewport, &dX, &dY, &dZ);
        vec2<float> sv2 = vec2<float>(dX, dY);

        float result = distToLineSegment(sv1, sv2, loc);
        if (result < dist) {
            dist = result;
            nearestEdge = (*e);
        }
    }
    return nearestEdge;

}

//convenience because of the conditional return value.
float distToLineSegment(vec2<float> end1, vec2<float> end2, vec2<float> point) {
    vec2<float> a = end2 - end1;
    vec2<float> b = point - end1;

    if (a.isZero()) {
        return (point - end1).getMagnitude();
    }

    float t = a.dot(b) / a.getMagnitude2();

    if (t < 0) {
        //_debugPoints.append(end1);
        return a.getMagnitude();
    } if (t > 1) {
        //_debugPoints.append(end2);
        return point.getDistance(end2);
    } else {
        //_debugPoints.append(end1 + (a * t));
        return (point - (end1 + (a * t))).getMagnitude();
    }
}


