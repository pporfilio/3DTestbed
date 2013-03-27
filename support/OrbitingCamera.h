
#ifndef ORBITINGCAMERA_H
#define ORBITINGCAMERA_H

#include <QMouseEvent>
#include "CS123Algebra.h"
#include "utils.h"
#include "camera.h"


// An orbiting perspective camera specified by a center,
// two angles, and a zoom factor
class OrbitCamera : public Camera
{
public:

    OrbitCamera();

    vec3<float> center, up;
    float angle_x;
    float angle_y;
    float fovy;
    float zoom;

    float near_clip;
    float far_clip;

    void reset();
    void mouseMove(const vec2<int> &delta);
    void mouseWheel(float delta, bool shift_modifier);
    vec3<float> getPos();
    vec3<float> getLook3();
    vec3<float> getUp3();
    Matrix4x4 getFilmToWorld();
    float getFOVY();
    float getNearClip();
    float getFarClip();
    int getType();
    CameraState* getState();
    void setState(CameraState *state);

};


#endif // ORBITINGCAMERA_H
