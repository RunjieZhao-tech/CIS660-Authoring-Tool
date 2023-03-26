#include "dualquat.h"

DualQuat::DualQuat(const glm::quat q1,const glm::quat q2)
    :realpart(q1),dualpart(q2)
{}
DualQuat::DualQuat(const glm::mat4 transMat){
    DualQuat dqT = DualQuat(glm::quat(1,0,0,0),glm::quat(0,0.5f*glm::vec3(transMat[3])));
    DualQuat dqR = DualQuat(glm::quat(glm::mat3(transMat)),glm::quat(0,0,0,0));
    DualQuat dq = dqT*dqR;
    realpart = dq.realpart;
    dualpart = dq.dualpart;
}

DualQuat DualQuat::operator*(const DualQuat& dq2)const{
    glm::quat q1r = this->realpart;
    glm::quat q1c = this->dualpart;
    glm::quat q2r = dq2.realpart;
    glm::quat q2c = dq2.dualpart;
    return DualQuat(q1r*q2r,q1r*q2c+q1c*q2r);
}
