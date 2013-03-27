#ifndef CAMERA_H
#define CAMERA_H

#include "CS123Algebra.h"

enum CameraType {
    GAME_CAMERA, ORBIT_CAMERA
};

struct CameraState {
    float fovy;
    float nearClip;
    float farClip;
    vec3<float> pos;
    vec3<float> look;
    vec3<float> up;
};



class Camera
{
public:
    virtual void mouseMove(const vec2<int> &delta) = 0;
    virtual void mouseWheel(float delta, bool shift_modifier) = 0;
    virtual vec3<float> getPos() = 0;
    virtual Matrix4x4 getFilmToWorld() = 0;
    virtual vec3<float> getLook3() = 0;
    virtual vec3<float> getUp3() = 0;
    virtual void reset() = 0;

    //parameters cast to void to suppress unused warnings.
    //Functions defined here so that subclasses do not need to define them.
    virtual void cameraMoveUp(bool positive, bool shift_modifier) {
        (void)positive; (void)shift_modifier;
    }

    virtual void cameraMoveLook(bool positive, bool shift_modifier) {
        (void)positive; (void)shift_modifier;
    }

    virtual void cameraMoveSide(bool positive, bool shift_modifier) {
        (void)positive; (void)shift_modifier;
    }

    virtual void cameraRotLook(bool positive, bool shift_modifier) {
        (void)positive; (void)shift_modifier;
    }

    //couldn't get these to work as just variables.
    virtual float getFOVY() = 0;
    virtual float getNearClip() = 0;
    virtual float getFarClip() = 0;
    virtual int getType() = 0;
    virtual CameraState* getState() = 0;
    virtual void setState(CameraState *state) = 0;

};


#endif // CAMERA_H
