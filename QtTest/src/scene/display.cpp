#include "display.h"
#include <QDebug>
#include <iostream>
VertexDisplay::VertexDisplay(OpenGLContext* mp_context)
    :Drawable(mp_context),m_vert(nullptr)
{};
void VertexDisplay::create()
{
    std::vector<GLuint> glIndices;
    std::vector<glm::vec4> glPos;
    std::vector<glm::vec4> glCol;
    if(m_vert!=nullptr){
        glIndices.push_back(0);
        glPos.push_back(glm::vec4(m_vert->pos,1));
        glCol.push_back(glm::vec4(1,1,1,1));
    }
    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, glIndices.size() * sizeof(GLuint), glIndices.data(), GL_STATIC_DRAW);
    count = glIndices.size();
    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glPos.size() * sizeof(glm::vec4), glPos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glCol.size() * sizeof(glm::vec4), glCol.data(), GL_STATIC_DRAW);
};
GLenum VertexDisplay::drawMode()
{
    return GL_POINTS;
};
void VertexDisplay::updateVertex(Vertex* v)
{
    this->destroy();
    this->m_vert = v;
    if(v!=nullptr){
        std::cout<< v->pos.x<<", " << v->pos.y<< ", " << v->pos.z<< ", " << std::endl;
    }
    this->create();
};

FaceDisplay::FaceDisplay(OpenGLContext* mp_context)
    :Drawable(mp_context),m_face(nullptr)
{};
void FaceDisplay::create()
{
    std::vector<GLuint> glIndices;
    std::vector<glm::vec4> glPos;
    std::vector<glm::vec4> glCol;
    //if not do it the mygl will not draw
    if(m_face!=nullptr){
        std::vector<HalfEdge*> hEdges = m_face->getHalfEdges();
        glm::vec4 dcolor(glm::vec3(1,1,1) - m_face->color,1);
        for(unsigned long i=0;i<hEdges.size();i++){
            std::pair<Vertex*,Vertex*> p = hEdges[i]->getVerts();
            glPos.push_back(glm::vec4(p.first->pos,1));
            glPos.push_back(glm::vec4(p.second->pos,1));
            glCol.push_back(dcolor);
            glCol.push_back(dcolor);
        }
        for(unsigned long i=0;i<glPos.size();i++){
            glIndices.push_back(i);
        }
    }

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, glIndices.size() * sizeof(GLuint), glIndices.data(), GL_STATIC_DRAW);
    count = glIndices.size();
    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glPos.size() * sizeof(glm::vec4), glPos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glCol.size() * sizeof(glm::vec4), glCol.data(), GL_STATIC_DRAW);
};
GLenum FaceDisplay::drawMode()
{
    return GL_LINES;
};
void FaceDisplay::updateFace(Face* f)
{
    this->destroy();
    m_face = f;
    this->create();
};

HalfEdgeDisplay::HalfEdgeDisplay(OpenGLContext* mp_context)
    :Drawable(mp_context),m_hEdge(nullptr)
{};
void HalfEdgeDisplay::create()
{
    std::vector<GLuint> glIndices;
    std::vector<glm::vec4> glPos;
    std::vector<glm::vec4> glCol;
    if(m_hEdge!=nullptr){
        std::pair<Vertex*,Vertex*> p = m_hEdge->getVerts();
        glPos.push_back(glm::vec4(p.first->pos,1));
        glPos.push_back(glm::vec4(p.second->pos,1));
        glPos.push_back(glm::vec4(1,1,1,1));
        glPos.push_back(glm::vec4(1,1,1,1));
        glIndices.push_back(0);
        glIndices.push_back(1);
        glCol.push_back(glm::vec4(1,0,0,1));
        glCol.push_back(glm::vec4(1,1,0,1));
    }

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, glIndices.size() * sizeof(GLuint), glIndices.data(), GL_STATIC_DRAW);
    count = glIndices.size();
    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glPos.size() * sizeof(glm::vec4), glPos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glCol.size() * sizeof(glm::vec4), glCol.data(), GL_STATIC_DRAW);
};
GLenum HalfEdgeDisplay::drawMode()
{
    return GL_LINES;
};
void HalfEdgeDisplay::updateHalfEdge(HalfEdge* e)
{
    this->destroy();
    m_hEdge = e;
    this->create();
};

JointDisplay::JointDisplay(OpenGLContext* mp_context)
    :Drawable(mp_context),m_joint(nullptr)
{};
void JointDisplay::createCircle(
        std::vector<GLuint>* out_ids,
        std::vector<glm::vec4>* out_col,
        std::vector<glm::vec4>* out_pos)
{
    int beginId = out_col->size();
    glm::vec4 base(0.5f,0,0,1);
    int edgeNum = 12;
    glm::mat4 rot = glm::rotate(glm::mat4(1.f),glm::radians(360.f/edgeNum),glm::vec3(0,0,1));
    for(int i=0;i<edgeNum;i++){
        out_col->push_back(glm::vec4());
        out_pos->push_back(base);
        out_ids->push_back(beginId+i);
        if((i+1)==edgeNum){
            out_ids->push_back(beginId);
        }else{
            out_ids->push_back(beginId+i+1);
        }
        base = rot*base;
    }
};
void JointDisplay::createJointDisplay(
        std::vector<GLuint>* out_ids,
        std::vector<glm::vec4>* out_col,
        std::vector<glm::vec4>* out_pos)
{
    //draw self
    glm::mat4 overallMat = m_joint->getOverallTransform();
    glm::mat4 rotRed = overallMat*glm::rotate(glm::mat4(1.f),glm::radians(90.f),glm::vec3(0,1,0));
    glm::mat4 rotGreen = overallMat*glm::rotate(glm::mat4(1.f),glm::radians(90.f),glm::vec3(1,0,0));
    glm::mat4 rotBlue = overallMat;
    int beginId = out_col->size();
    int endId = 0;
    createCircle(out_ids,out_col,out_pos);
    endId = out_col->size();
    for(int i=beginId;i<endId;i++){
        (*out_col)[i] = glm::vec4(1,1,0,1);
        (*out_pos)[i] = rotBlue*(*out_pos)[i];
    }

    beginId = endId;
    createCircle(out_ids,out_col,out_pos);
    endId = out_col->size();
    for(int i=beginId;i<endId;i++){
        (*out_col)[i] = glm::vec4(1,1,0,1);
        (*out_pos)[i] = rotRed*(*out_pos)[i];
    }

    beginId = endId;
    createCircle(out_ids,out_col,out_pos);
    endId = out_col->size();
    for(int i=beginId;i<endId;i++){
        (*out_col)[i] = glm::vec4(1,1,0,1);
        (*out_pos)[i] = rotGreen*(*out_pos)[i];
    }
};
GLenum JointDisplay::drawMode()
{
    return GL_LINES;
};
void JointDisplay::create(){
    std::vector<GLuint> glIndices;
    std::vector<glm::vec4> glPos;
    std::vector<glm::vec4> glCol;
    if(m_joint!=nullptr){
        createJointDisplay(&glIndices,&glCol,&glPos);
    }

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, glIndices.size() * sizeof(GLuint), glIndices.data(), GL_STATIC_DRAW);
    count = glIndices.size();
    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glPos.size() * sizeof(glm::vec4), glPos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glCol.size() * sizeof(glm::vec4), glCol.data(), GL_STATIC_DRAW);
}
void JointDisplay::updateJoint(const SkeletonJoint* root){
    destroy();
    m_joint = root;
    create();
}

SkinDisplay::SkinDisplay(OpenGLContext* mp_context)
    :Drawable(mp_context),m_mesh(nullptr),jointId(-1)
{}
void SkinDisplay::create(){

    std::vector<GLuint>    glIndices;
    std::vector<glm::vec4> glPos;
    std::vector<glm::vec4> glCol;
    std::vector<glm::vec4> glNor;
    std::vector<int>       glJointIds;
    std::vector<float>     glWeights;
    if(m_mesh!=nullptr){
        m_mesh->cloneDraw(&glIndices,&glPos,&glCol,&glNor,&glJointIds,&glWeights);
    }
    if(jointId==-1){
        for(unsigned int i=0;i<glCol.size();i++){
            glCol[i] = glm::vec4(0,0,0,1.f);
        }
    }else{
        int numJointPerVert = glJointIds.size()/glCol.size();//number of joints per vertex
        for(unsigned int i=0;i<glCol.size();i++){
            glCol[i] = glm::vec4(0,0,0,1.f);
            for(int j=0;j<numJointPerVert;j++){
                int idx = i*numJointPerVert+j;
                if(jointId==glJointIds[idx]){
                    glCol[i] = glm::vec4(glWeights[idx],glWeights[idx],glWeights[idx],1.f);
                }
            }
        }
    }
    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, glIndices.size() * sizeof(GLuint), glIndices.data(), GL_STATIC_DRAW);
    count = glIndices.size();
    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glPos.size() * sizeof(glm::vec4), glPos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glCol.size() * sizeof(glm::vec4), glCol.data(), GL_STATIC_DRAW);

    generateNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glNor.size() * sizeof(glm::vec4), glNor.data(), GL_STATIC_DRAW);

    generateJointIds();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufJointIds);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glJointIds.size() * sizeof(int), glJointIds.data(), GL_STATIC_DRAW);

    generateWeights();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufWeights);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glWeights.size() * sizeof(float), glWeights.data(), GL_STATIC_DRAW);
}
void SkinDisplay::updateMeshJoint(const Mesh* m,int j){
    m_mesh = m;
    jointId = j;
    destroy();
    create();
}

PatchDisplay::PatchDisplay(OpenGLContext* mp_context)
    :Drawable(mp_context),m_patch(nullptr)
{}
void PatchDisplay::create(){
    std::vector<GLuint>    glIndices;
    std::vector<glm::vec4> glPos;
    std::vector<glm::vec4> glCol;
    std::vector<glm::vec4> glNor;

    if(m_patch==nullptr)return;
    for(unsigned long i=0;i<m_patch->quads.size();i++){
        Face* f = m_patch->quads[i].get();
        int ids = glPos.size();
        std::vector<HalfEdge*> edges = f->getHalfEdges();
        int size = edges.size()-2;
        for(int i =0;i<size;i++){
            Vertex* v0 = edges[0]->vert;
            Vertex* v1 = edges[i+1]->vert;
            Vertex* v2 = edges[i+2]->vert;
            Vertex* v[3] ={v0,v1,v2};
            glm::vec4 norm = glm::vec4(glm::cross(v1->pos-v0->pos,v2->pos-v1->pos),0);

            for(int j=0;j<3;j++){
               Vertex* vert = v[j];
               glPos.push_back(glm::vec4(vert->pos,1));
               glCol.push_back(glm::vec4(f->color,1));
               glNor.push_back(norm);
               glIndices.push_back(ids++);
            }
        }
    }
    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, glIndices.size() * sizeof(GLuint), glIndices.data(), GL_STATIC_DRAW);
    count = glIndices.size();
    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glPos.size() * sizeof(glm::vec4), glPos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glCol.size() * sizeof(glm::vec4), glCol.data(), GL_STATIC_DRAW);

    generateNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, glNor.size() * sizeof(glm::vec4), glNor.data(), GL_STATIC_DRAW);

}

void PatchDisplay::updatePatch(Patch* p){
    destroy();
    m_patch = p;
    create();
}
