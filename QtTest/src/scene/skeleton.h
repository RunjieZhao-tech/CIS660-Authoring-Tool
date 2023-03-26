#pragma once
#include <QTreeWidgetItem>
#include "la.h"
#include "drawable.h"
#include "smartpointerhelp.h"
class SkeletonJoint:public QTreeWidgetItem
{
private:
    SkeletonJoint* parent;
    std::vector<SkeletonJoint*> children;
public:
    glm::vec3 prPos;
    glm::quat rot;
    glm::mat4 bindMat;

    SkeletonJoint();
    ~SkeletonJoint();

    SkeletonJoint(std::string name,
                  SkeletonJoint* parent,
                  std::vector<SkeletonJoint*>* children,
                  glm::vec3 pos,
                  glm::quat rot,
                  glm::mat4 bind);
    SkeletonJoint(std::string name,
                  glm::vec3 pos,
                  glm::quat rot,
                  glm::mat4 bind);
    void setName(std::string name);
    std::string getName()const;
    //these two methods will also inform the child that
    //the joint is its parent
    void addChild(SkeletonJoint* child);
    void addChildren(std::vector<SkeletonJoint*>* children);
    const std::vector<SkeletonJoint*>* getChildren()const;

    glm::mat4 getLocalTransform() const;
    glm::mat4 getOverallTransform() const;
};

class Skeleton: public Drawable
{

private:
    SkeletonJoint* m_root;
    std::vector<uPtr<SkeletonJoint>> joints;
    //helper functions to construct sphere display
    void createCircle(
            std::vector<GLuint>* out_ids,
            std::vector<glm::vec4>* out_col,
            std::vector<glm::vec4>* out_pos);
    void createJointDisplay(
            const SkeletonJoint* currJoint,
            glm::mat4 overallMat,
            std::vector<GLuint>* out_ids,
            std::vector<glm::vec4>* out_col,
            std::vector<glm::vec4>* out_pos);
    void createJoint(const QJsonObject& obj,SkeletonJoint* parent);//recursive
    void clearSkeleton();
public:
    Skeleton(OpenGLContext* mp_context);
    void create() override;
    GLenum drawMode() override;
    void createFromJSON(const char* filename);
    friend class MyGL;
};
