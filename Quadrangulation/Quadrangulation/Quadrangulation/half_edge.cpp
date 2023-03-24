#include "half_edge.h"
#include<iostream>
Vertex::Vertex(glm::vec3 pos, HalfEdge* hEdge)
    :hEdge(hEdge), pos(pos)
{}

Vertex::Vertex()
    :Vertex(glm::vec3(0, 0, 0), nullptr)
{}

std::vector<HalfEdge*> Vertex::getHalfEdges() {
    std::vector<HalfEdge*> res;
    HalfEdge* current = hEdge;
    do {
        res.push_back(current);
        current = current->nHEdge->sHEdge;
    } while (current != hEdge);
    return res;
}//return all half edges point to it

HalfEdge::HalfEdge(HalfEdge* nHEdge, HalfEdge* sHEdge, Face* face, Vertex* vert)
    :nHEdge(nHEdge), sHEdge(sHEdge), face(face), vert(vert)
{
    if (sHEdge != nullptr) {
        sHEdge->sHEdge = this;
    }
    if (face != nullptr) {
        face->hEdge = this;
    }
    if (vert != nullptr) {
        vert->hEdge = this;
    }
};

//copy the data from one edge to another
HalfEdge::HalfEdge(HalfEdge *edge) {
    this->face = edge->face;
    this->nHEdge = edge->nHEdge;
    this->sHEdge = edge->sHEdge;
    this->vert = edge->vert;
}

HalfEdge::HalfEdge()
    :HalfEdge(nullptr, nullptr, nullptr, nullptr)
{}

void HalfEdge::setFace(Face* iface) {
    this->face = iface;
    if (iface != nullptr) {
        iface->hEdge = this;
    }
}
void HalfEdge::setVert(Vertex* ivert) {
    this->vert = ivert;
    if (ivert != nullptr) {
        ivert->hEdge = this;
    }
}

//set symmetric edge
void HalfEdge::setSymEdge(HalfEdge* iEdge) {
    this->sHEdge = iEdge;
    if (iEdge != nullptr) {
        iEdge->sHEdge = this;
    }
}

//set next edge
void HalfEdge::setNextEdge(HalfEdge* nEdge) {
    this->nHEdge = nEdge;
}

Face* HalfEdge::getFace() {
    return face;
}

//get symmetric edge
HalfEdge* HalfEdge::getSymEdge() {
    return this->sHEdge;
}

std::pair<Vertex*, Vertex*> HalfEdge::getVerts() {
    return std::pair<Vertex*, Vertex*>(sHEdge->vert, vert);
}

Face::Face(HalfEdge* hEdge, glm::vec3 color)
    :hEdge(hEdge), color(color)
{
    if (hEdge != nullptr) {
        hEdge->setFace(this);
    }
}
Face::Face()
    :Face(nullptr, glm::vec3(rand() % 100 / 100.f, rand() % 100 / 100.f, rand() % 100 / 100.f))
{}
void Face::triangulate(std::vector<Vertex*>* outVerts, std::vector<unsigned int>* outIds) {
    unsigned int start = outVerts->size();
    std::vector<HalfEdge*> edges = getHalfEdges();
    for (HalfEdge* edge : edges) {
        outVerts->push_back(edge->vert);
    }
    int size = edges.size() - 2;
    for (int i = 0;i < size;i++) {
        outIds->push_back(start + 0);
        outIds->push_back(start + i + 1);
        outIds->push_back(start + i + 2);
    }
}

//Test if a point is in an triangle
bool inTriangle(glm::vec3 testPt, glm::vec3 trigPt0, glm::vec3 trigPt1, glm::vec3 trigPt2, glm::vec3 norm) {
    float s1 = abs(glm::dot(glm::cross(testPt - trigPt0, testPt - trigPt1), norm));
    float s2 = abs(glm::dot(glm::cross(testPt - trigPt1, testPt - trigPt2), norm));
    float s3 = abs(glm::dot(glm::cross(testPt - trigPt2, testPt - trigPt0), norm));
    float s = abs(glm::dot(glm::cross(trigPt2 - trigPt0, trigPt2 - trigPt1), norm));
    return s >= (s1 + s2 + s3);
}
std::vector<Vertex*> Face::getVertices() {
    std::vector<Vertex*> verts;
    std::vector<HalfEdge*> edges = getHalfEdges();
    for (HalfEdge* edge : edges) {
        verts.push_back(edge->vert);
    }
    return verts;
}
glm::vec3 Face::getCenter() {
    std::vector<Vertex*> verts = getVertices();
    unsigned long count = verts.size();
    glm::vec3 res(0, 0, 0);
    for (unsigned long i = 0;i < verts.size();i++) {
        res += verts[i]->pos;
    }
    res = 1.f / count * res;
    return res;
}
std::vector<HalfEdge*> Face::getHalfEdges() {
    std::vector<HalfEdge*> res;
    HalfEdge* current = hEdge;
    do {
        res.push_back(current);
        current = current->nHEdge;
    } while (current != hEdge);
    return res;
}//return all half edges around it

//set the edge that face will point to 
void Face::setEdge(HalfEdge* e) {
    this->hEdge = e;
}