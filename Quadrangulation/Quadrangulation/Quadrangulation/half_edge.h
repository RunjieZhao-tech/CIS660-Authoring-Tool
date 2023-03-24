#pragma once
#include <la.h>
class HalfEdge;

class Vertex{
private:
    HalfEdge* hEdge;//A pointer to one of the HalfEdges that points to this Vertex
public:
    glm::vec3 pos;//A vec3 for storing its position
    Vertex(glm::vec3 pos, HalfEdge* hEdge);
    Vertex();
    std::vector<HalfEdge*> getHalfEdges();//return all half edges point to it
    friend class HalfEdge;
    friend class Face;
};

class Face {
private:
    HalfEdge* hEdge;//A pointer to one of the HalfEdges that lies on this Face
public:
    glm::vec3 color;//A vec3 to represent this Face's color as an RGB value
    Face(HalfEdge* hEdge, glm::vec3 color);
    Face();
    std::vector<HalfEdge*> getHalfEdges();//return all half edges around it
    std::vector<Vertex*> getVertices();
    void setEdge(HalfEdge*); //set the one of edges that this Face points to
    void triangulate(std::vector<Vertex*>* verts, std::vector<unsigned int>* ids);
    glm::vec3 getCenter();
    friend class HalfEdge;
    friend class Vertex;
};

class HalfEdge {
private:
    HalfEdge* nHEdge;//A pointer to the next HalfEdge in the loop of HalfEdges that lie on this HalfEdge's Face
    HalfEdge* sHEdge;//A pointer to the HalfEdge that lies parallel to this HalfEdge and which travels in the opposite direction and is part of an adjacent Face, i.e. this HalfEdge's symmetrical HalfEdge
    Face* face;//A pointer to the Face on which this HalfEdge lies
    Vertex* vert;//A pointer to the Vertex between this HalfEdge and its next HalfEdge
public:
    HalfEdge(HalfEdge* nHEdge, HalfEdge* sHEdge, Face* face, Vertex* vert);
    HalfEdge(HalfEdge* edge);
    HalfEdge();
    void setFace(Face* iface);
    void setVert(Vertex* ivert);
    void setSymEdge(HalfEdge* iEdge);
    void setNextEdge(HalfEdge* nEdge);
    Face* getFace();
    HalfEdge* getSymEdge();
    //get two vertices of the edge, second is the vert the half edge point to
    std::pair<Vertex*, Vertex*> getVerts();
    friend class Face;
    friend class Vertex;
};

struct HEdgeHash {
    uint64_t operator()(std::pair<Vertex*, Vertex*> p)const {
        uint64_t addr1 = reinterpret_cast<uint64_t>(p.first);
        uint64_t addr2 = reinterpret_cast<uint64_t>(p.second);
        return addr1 ^ addr2;
    }
};

