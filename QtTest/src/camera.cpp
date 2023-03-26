#include "camera.h"

#include <la.h>
#include <iostream>


Camera::Camera():
    Camera(618, 432)
{
    look = glm::vec3(0,0,-1);
    up = glm::vec3(0,1,0);
    right = glm::vec3(1,0,0);
}

Camera::Camera(unsigned int w, unsigned int h):
    Camera(w, h, glm::vec3(0,0,10), glm::vec3(0,0,0), glm::vec3(0,1,0))
{}

Camera::Camera(unsigned int w, unsigned int h, const glm::vec3 &e, const glm::vec3 &r, const glm::vec3 &worldUp):
    fovy(45),
    width(w),
    height(h),
    near_clip(0.1f),
    far_clip(1000),
    eye(e),
    ref(r),
    world_up(worldUp)
{
    RecomputeAttributes();
}

Camera::Camera(const Camera &c):
    fovy(c.fovy),
    width(c.width),
    height(c.height),
    near_clip(c.near_clip),
    far_clip(c.far_clip),
    aspect(c.aspect),
    eye(c.eye),
    ref(c.ref),
    look(c.look),
    up(c.up),
    right(c.right),
    world_up(c.world_up),
    V(c.V),
    H(c.H)
{}


void Camera::RecomputeAttributes()
{
    look = glm::normalize(ref - eye);
    right = glm::normalize(glm::cross(look, world_up));
    up = glm::cross(right, look);

    float tan_fovy = tan(glm::radians(fovy/2));
    float len = glm::length(ref - eye);
    aspect = width / static_cast<float>(height);
    V = up*len*tan_fovy;
    H = right*len*aspect*tan_fovy;
}

glm::mat4 Camera::getViewProj()
{
    return glm::perspective(glm::radians(fovy), width / (float)height, near_clip, far_clip) * glm::lookAt(eye, ref, up);
}

void Camera::RotateAboutUp(float deg)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), up);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}
void Camera::RotateAboutRight(float deg)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), right);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}

void Camera::TranslateAlongLook(float amt)
{
    glm::vec3 translation = look * amt;
    eye += translation;
    ref += translation;
}

void Camera::TranslateAlongRight(float amt)
{
    glm::vec3 translation = right * amt;
    eye += translation;
    ref += translation;
}
void Camera::TranslateAlongUp(float amt)
{
    glm::vec3 translation = up * amt;
    eye += translation;
    ref += translation;
}

bool Camera::RayCast(const glm::vec2 screenpoint,HitInfo* info, const std::vector<Collider*> *colliders){

    float sx = screenpoint.x*2.f/width - 1;
    float sy = 1 - (2.f*screenpoint.y/height);
    glm::vec3 dir = ref + sx*H + sy*V - eye;
    dir = glm::normalize(dir);
    bool hit = false;
    for(Collider *collider: *colliders){
        HitInfo cInfo(eye,nullptr);
        bool hitWithThis = collider->hitTest(eye,dir,&cInfo);
        if(hitWithThis){
            /*update the info if hit for the first time or the new hit point is closer*/
            bool closer = !hit||(glm::length(info->hitPos-eye) > glm::length(cInfo.hitPos-eye));
            if(closer){
                *info = cInfo;
            }
        }
        hit = hit || hitWithThis;
    }
    return hit;
};

void Camera::PolarMoveVerticle(float deg){
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), right);
    eye = eye - ref;
    eye = glm::vec3(rotation*glm::vec4(eye,1));
    eye = eye + ref;
    RecomputeAttributes();
};
void Camera::PolarMoveHorizontal(float deg){
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), world_up);
    eye = eye - ref;
    eye = glm::vec3(rotation*glm::vec4(eye,1));
    eye = eye + ref;
    RecomputeAttributes();
};
void Camera::PolarZoom(float amt){
    glm::vec3 translation = look * amt;
    eye += translation;
    RecomputeAttributes();
};

