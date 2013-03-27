#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <math.h>
#include "camera.h"
#include "../geom/polygon.h"
#include "glwidget.h"

class GLWidget;

//Compute once the value of pi to double precision and store for use elsewhere.
const double PI = 4.0 * atan(1.0);

float maxN(int n, ...);

float minN(int n, ...);

double toRadians(double degrees);

void printMat4x4(const Matrix4x4 &m);

void printVec3(const vec3<float> &v, bool newline=false);

void printVec4(const Vector4 &v, bool newline=false);

void QStringToChar(QString *qstr, char *dst_arr);

std::vector<vec3<REAL> > getPointsOnCircle(vec3<REAL> center, double radius, int n);

std::vector<vec3<REAL> > getPointsOnSphere(vec3<REAL> center, double radius,
                                    int n_sides, int n_vertical_divisions);

vec3<REAL> clamp(vec3<REAL> v, double min, double max);

QString myGetSaveFileName(QWidget *parent, const QString &caption,
                                    const QString &dir, const QString &filter,
                                    bool save);

vec3<float> centroid(const std::vector<Vertex *> points);

void push2D(GLWidget *widget);

void pop2D();

Edge * getNearestScreenspaceEdge(std::vector<Edge *> edges, vec2<float> loc, float &dist);
float distToLineSegment(vec2<float> end1, vec2<float> end2, vec2<float> point);


#endif // UTILS_H
