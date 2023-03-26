#pragma once
#include "halfedge.h"
#include "smartpointerhelp.h"
#include "drawable.h"
#include <la.h>
#include <unordered_map>
class Mesh:public Drawable
{
private:
    std::vector<uPtr<Face>> faces;
    std::vector<uPtr<HalfEdge>> hEdges;
    std::vector<uPtr<Vertex>> verts;
    float m_sharpness;

    void triangulate(Face* face,std::vector<GLuint>* outIds, std::vector<glm::vec4>* outPos,
                     std::vector<glm::vec4>* outCol, std::vector<glm::vec4>* outNor,
                     std::vector<int>* outJIds,std::vector<float>* outWeights)const;
    void buildFace(const std::vector<glm::vec3>* pos,const std::vector<int>* face);
    void subDivide(HalfEdge* halfedge,const std::vector<glm::vec3>* centers);
    void quadrangulate(Face* face,const glm::vec3 center,std::vector<Vertex*>* midPts);
    void processSYM(std::unordered_map<std::pair<Vertex*,Vertex*>,HalfEdge*,HEdgeHash>*map,HalfEdge* hEdge,Vertex* sV,Vertex* eV);
public:
    Mesh(OpenGLContext* mp_context);
    void createFromOBJ(const char* filename);
    void create();
    friend class MyGL;

    void addMidPoint(HalfEdge* halfedge, HalfEdge* &newHalfEdge, HalfEdge* &newSymEdge, Vertex* &newVert);
    void splitFace(Face* face, std::vector<Face*>* newFaces, std::vector<HalfEdge*>* newHalfEdges);
    void CCSubdivision(std::vector<Face*>* out_newFaces,std::vector<HalfEdge*>* out_newHalfEdges, std::vector<Vertex*>* out_newVerts);
    void extrudeFace(Face* face,float len,std::vector<Face*>* out_newFaces,std::vector<HalfEdge*>* out_newHalfEdges, std::vector<Vertex*>* out_newVerts);
    void setSharpness(float input);
    void cloneDraw(std::vector<GLuint>* outIds, std::vector<glm::vec4>* outPos,
                   std::vector<glm::vec4>* outCol, std::vector<glm::vec4>* outNor,
                   std::vector<int>* outJIds,std::vector<float>* outWeights)const;
};
