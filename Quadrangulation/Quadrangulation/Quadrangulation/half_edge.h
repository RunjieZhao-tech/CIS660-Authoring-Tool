#pragma once
#include "la.h"
#include "smartpointerhelp.h"
#include <unordered_map>
class HalfEdge;

class Vertex{
public:
    HalfEdge* hEdge;//A pointer to one of the HalfEdges that points to this Vertex
    glm::vec3 pos;//A vec3 for storing its position
    Vertex(glm::vec3 pos, HalfEdge* hEdge);
    Vertex();
    std::vector<HalfEdge*> getHalfEdges();//return all half edges point to it
    friend class HalfEdge;
    friend class Face;
};

class Face {
protected:
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
    //this method will return adjacent quads, 
    //begins with the quad that has the symmetry halfedge of the input halfedge
    //and then other 3 quads in counterclockwise order
    std::vector<Face*> getAdjacentFaces(HalfEdge* halfEdge);
    //       --------------------------
    //       |            |           |
    //  get<---- Quad     |   Quad    |------->give
    //       |            |           |
    //       --------------------------
    /*Face* getQuadOppositeTo(HalfEdge* halfEdge);*/
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

    HalfEdge(HalfEdge* edge);
    HalfEdge(HalfEdge* nHEdge, HalfEdge* sHEdge, Face* face, Vertex* vert);
    HalfEdge();
    void setFace(Face* iface);
    void setVert(Vertex* ivert);
    void setSymEdge(HalfEdge* iEdge);
    void setNextEdge(HalfEdge* nEdge);

    Face* getFace();
    HalfEdge* getNextHalfEdge();
    HalfEdge* getSymHalfEdge();

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

//This is the class that we want to fill tiles in
class Patch {
public:
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
    //helper function to subdivide quad, 
    //inputs are the new generated vertices on the shared side of the adjacent quad, 
    //and the adjacent quad's halfedge that originally shares that side
    //outputs are the new generated vertices on the other side of this quad,
    //and the oppoist edge for next adjacent quad 
    //this function only handles halfedges that are in the quad
    HalfEdge* subDivideQuad(
        std::vector<Vertex*>& newVerts, 
        HalfEdge* adjacentHalfEdge, 
        std::vector<Vertex*>& out_newVerts)
    ////To display patch's quads
    //void createDrawable(std::vector<int> glIndices, 
    //                    std::vector<glm::vec4> glPos,
    //                    std::vector<glm::vec4> glCol);
};

