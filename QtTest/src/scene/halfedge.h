#pragma once
#include <la.h>
#include <QListWidgetItem>
#include "skeleton.h"
class HalfEdge;
struct Collider;

struct HitInfo{
    glm::vec3 hitPos;
    Collider* collider;
    HitInfo(glm::vec3 pos, Collider *c)
        :hitPos(pos),collider(c)
    {};
    HitInfo()
        :HitInfo(glm::vec3(0,0,0),nullptr)
    {};
};

struct Collider{
    virtual bool hitTest(glm::vec3 origin, glm::vec3 dirc, HitInfo* info)=0;
};

//A vec3 for storing its position
//A pointer to one of the HalfEdges that points to this Vertex
//A unique integer to identify the Vertex in menus and while debugging
class Vertex:public QListWidgetItem,public Collider{
private:
    static int count;
    bool sharp;
public:
    float radius;
    const int id;
    std::vector<int> jointIds;
    std::vector<float> jointWeights;
    bool hitTest(glm::vec3 origin, glm::vec3 dirc, HitInfo* info)override;
    void setSharp(bool v);
    //return if joint will effect vertex
    //if the new weight is 1, vertex will be changed anyway
    bool bindJoint(int id,float weight);
    void unBindJoints();
    friend class Mesh;
    friend class MyGL;
    friend class HalfEdge;
    friend class Face;
private:

public:
    HalfEdge* hEdge;
    glm::vec3 pos;
    Vertex(glm::vec3 pos, HalfEdge* hEdge);
    Vertex();
    std::vector<HalfEdge*> getHalfEdges();//return all half edges point to it
};

//A pointer to one of the HalfEdges that lies on this Face
//A vec3 to represent this Face's color as an RGB value
//A unique integer to identify the Face in menus and while debugging
class Face:public QListWidgetItem,public Collider{
private:
    static int count;
    bool sharp;
public:

    const int id;

    bool hitTest(glm::vec3 origin, glm::vec3 dirc, HitInfo* info)override;
    void triangulate(std::vector<Vertex*>* verts,std::vector<unsigned int>* ids);
    void setSharp(bool v);
    friend class Mesh;
    friend class MyGL;
    friend class HalfEdge;
    friend class Vertex;

private:
    HalfEdge * hEdge;
public:
    glm::vec3 color;
    Face(HalfEdge* hEdge, glm::vec3 color);
    Face();
    std::vector<HalfEdge*> getHalfEdges();//return all half edges around it
    std::vector<Vertex*> getVertices();
    void setEdge(HalfEdge*);
    glm::vec3 getCenter();
    //this method will return adjacent quads,
    //begins with the quad that has the symmetry halfedge of the input halfedge
    //and then other 3 quads in counterclockwise order
    std::vector<Face*> getAdjacentFaces(HalfEdge* halfEdge);
};

//A pointer to the next HalfEdge in the loop of HalfEdges that lie on this HalfEdge's Face
//A pointer to the HalfEdge that lies parallel to this HalfEdge and which travels in the opposite direction and is part of an adjacent Face, i.e. this HalfEdge's symmetrical HalfEdge
//A pointer to the Face on which this HalfEdge lies
//A pointer to the Vertex between this HalfEdge and its next HalfEdge
//A unique integer to identify the HalfEdge in menus and while debugging
class HalfEdge:public QListWidgetItem,public Collider{
private:
    bool sharp;
    static int count;
public:
    float radius;//cylinder's radius
    const int id;
    bool hitTest(glm::vec3 origin, glm::vec3 dirc, HitInfo* info)override;//cylinder collider
    void setSharp(bool v);
    friend class Mesh;
    friend class MyGL;
    friend class Face;
    friend class Vertex;

private:
    HalfEdge* nHEdge;
    HalfEdge* sHEdge;
    Face* face;

public:
    Vertex* vert;
    HalfEdge(HalfEdge* nHEdge, HalfEdge* sHEdge, Face* face, Vertex* vert);
    HalfEdge();
    void setFace(Face* iface);
    void setVert(Vertex* ivert);
    void setSymEdge(HalfEdge* iEdge);
    void setNextEdge(HalfEdge* nEdge);
    HalfEdge* getNextHalfEdge();
    HalfEdge* getSymHalfEdge();
    Face* getFace();
    std::pair<Vertex*,Vertex*> getVerts();//get two vertices of the edge, second is the vert the half edge point to
};

struct HEdgeHash{
    uint64_t operator()(std::pair<Vertex*,Vertex*> p)const{
        uint64_t addr1 = reinterpret_cast<uint64_t>(p.first);
        uint64_t addr2 = reinterpret_cast<uint64_t>(p.second);
        return addr1^addr2;
    }
};

//This is the class that we want to fill tiles in
class Patch {
public:
    //only for test
    Patch();
    //**********************
    Patch(std::vector<glm::vec3> vertices);
    std::vector<std::pair<Vertex*,Vertex*>> sides;//only the vertices on the boundary in a counterclockwise order
    std::vector<uPtr<Face>> quads;//specific faces only has 4 sides
    std::vector<uPtr<HalfEdge>>hEdges;
    std::vector<uPtr<Vertex>> verts;
    bool hasQuad;//check if already quadrangulated
    bool isConvex();//check if the path is convex, we only consider convex patch in our implementation
    //helper function for setting up halfedge
    //return true if it sets sym for halfedge, return false if it doesn't
    bool processSYM(
        std::unordered_map<std::pair<Vertex*, Vertex*>, HalfEdge*, HEdgeHash>* map,
        HalfEdge* hEdge, Vertex* sV, Vertex* eV);

    void quadrangulate();
    //this method will return new generated quad after division
    void subDivide(HalfEdge* edgeToDivide, int divideNum);
    //To display patch's quads
    void createDrawable(std::vector<int> glIndices,
                        std::vector<glm::vec4> glPos,
                        std::vector<glm::vec4> glCol);
};
