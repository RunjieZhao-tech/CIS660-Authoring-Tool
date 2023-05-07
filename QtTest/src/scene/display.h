#pragma once
#include "drawable.h"
#include "halfedge.h"
#include "skeleton.h"
#include "mesh.h"
class VertexDisplay:public Drawable
{
private:
    Vertex* m_vert;
public:
    VertexDisplay(OpenGLContext* mp_context);
    void create() override;
    GLenum drawMode() override;
    void updateVertex(Vertex*);
};

class FaceDisplay:public Drawable
{
private:
    Face* m_face;
public:
    FaceDisplay(OpenGLContext* mp_context);
    void create() override;
    GLenum drawMode() override;
    void updateFace(Face*);
};

class HalfEdgeDisplay:public Drawable
{
private:
    HalfEdge* m_hEdge;
public:
    HalfEdgeDisplay(OpenGLContext* mp_context);
    void create() override;
    GLenum drawMode() override;
    void updateHalfEdge(HalfEdge*);
};

class JointDisplay:public Drawable
{
private:
    const SkeletonJoint* m_joint;
    //helper functions to construct sphere display
    void createCircle(
            std::vector<GLuint>* out_ids,
            std::vector<glm::vec4>* out_col,
            std::vector<glm::vec4>* out_pos);
    void createJointDisplay(
            std::vector<GLuint>* out_ids,
            std::vector<glm::vec4>* out_col,
            std::vector<glm::vec4>* out_pos);
public:
    JointDisplay(OpenGLContext* mp_context);
    void create() override;
    GLenum drawMode() override;
    void updateJoint(const SkeletonJoint*);
};

class SkinDisplay:public Drawable
{
private:
    const Mesh* m_mesh;
    int jointId;
public:
    SkinDisplay(OpenGLContext* mp_context);
    void create() override;
    void updateMeshJoint(const Mesh*,int);
};


class PatchDisplay:public Drawable
{
public:
    Patch* m_patch;
    PatchDisplay(OpenGLContext* mp_context);
    void create() override;
    //GLenum drawMode() override;
    void updatePatch(Patch*);
};
