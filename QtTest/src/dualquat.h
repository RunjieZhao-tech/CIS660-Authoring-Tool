#ifndef DUALQUAT_H
#define DUALQUAT_H
#include "la.h"

class DualQuat
{
public:
    glm::quat realpart;
    glm::quat dualpart;
    DualQuat(const glm::quat,const glm::quat);
    DualQuat operator*(const DualQuat& dq2)const;
    DualQuat(const glm::mat4 transMat);
};

#endif // DUALQUAT_H
