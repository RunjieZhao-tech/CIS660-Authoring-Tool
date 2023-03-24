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

HalfEdge::HalfEdge(HalfEdge* nHEdge, HalfEdge* sHEdge, Quad* face, Vertex* vert)
    :nHEdge(nHEdge), sHEdge(sHEdge), quad(face), vert(vert)
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
HalfEdge::HalfEdge()
    :HalfEdge(nullptr, nullptr, nullptr, nullptr)
{}
void HalfEdge::setQuad(Quad* iface) {
    this->quad = iface;
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
void HalfEdge::setNextEdge(HalfEdge* nEdge) {
    this->nHEdge = nEdge;
}
HalfEdge* HalfEdge::getNextHalfEdge() {
    return this->nHEdge;
}
HalfEdge* HalfEdge::getSymHalfEdge() {
    return this->sHEdge;
}
Quad* HalfEdge::getQuad() {
    return quad;
}
std::pair<Vertex*, Vertex*> HalfEdge::getVerts() {
    return std::pair<Vertex*, Vertex*>(sHEdge->vert, vert);
}

Quad::Quad(HalfEdge* hEdge, glm::vec3 color)
    :hEdge(hEdge), color(color)
{
    if (hEdge != nullptr) {
        hEdge->setQuad(this);
    }
}
Quad::Quad()
    :Quad(nullptr, glm::vec3(rand() % 100 / 100.f, rand() % 100 / 100.f, rand() % 100 / 100.f))
{}
void Quad::setEdge(HalfEdge* e) {
    this->hEdge = e;
}
void Quad::triangulate(std::vector<Vertex*>* outVerts, std::vector<unsigned int>* outIds) {
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
std::vector<Vertex*> Quad::getVertices() {
    std::vector<Vertex*> verts;
    std::vector<HalfEdge*> edges = getHalfEdges();
    for (HalfEdge* edge : edges) {
        verts.push_back(edge->vert);
    }
    return verts;
}
glm::vec3 Quad::getCenter() {
    std::vector<Vertex*> verts = getVertices();
    unsigned long count = verts.size();
    glm::vec3 res(0, 0, 0);
    for (unsigned long i = 0;i < verts.size();i++) {
        res += verts[i]->pos;
    }
    res = 1.f / count * res;
    return res;
}
std::vector<HalfEdge*> Quad::getHalfEdges() {
    std::vector<HalfEdge*> res;
    HalfEdge* current = hEdge;
    do {
        res.push_back(current);
        current = current->nHEdge;
    } while (current != hEdge);
    return res;
}//return all half edges around it
std::vector<Quad*> Quad::getAdjacentQuads(HalfEdge* halfEdge) {
    std::vector<Quad*> result;
    HalfEdge* curr = halfEdge;
    do {
        if (curr->sHEdge != nullptr) {
            result.push_back(curr->sHEdge->quad);
        }
        else {
            result.push_back(nullptr);
        }
        curr = curr->nHEdge;
    } while (curr != halfEdge);
    return result;
}
Quad* Quad::getQuadOppositeTo(HalfEdge* halfEdge) {
    HalfEdge* inQuadEdge = halfEdge;
    if (inQuadEdge->getQuad() != this) {
        inQuadEdge = inQuadEdge->sHEdge;
    }
    HalfEdge* sym = inQuadEdge->nHEdge->nHEdge;
    if (sym != nullptr) {
        return sym->getQuad();
    }
    return nullptr;
}

Patch::Patch(std::vector<glm::vec3> vertices)
    :hasQuad(false)
{
    //*********initialize verts***************
    for (glm::vec3 pos : vertices) {
        verts.push_back(mkU<Vertex>(pos, nullptr));
    }
    //****************************************

    //***********initialize sides*************
    for (int i = 0;i < verts.size();i++) {
        int index1 = i;
        int index2 = (i+1) % verts.size();
        sides.push_back(std::pair<Vertex*, Vertex*>(verts[index1].get(), verts[index2].get()));
    }
    //******************************************
     
    //***********initialize halfedges*************
    // leave this in quadrangulation
    //******************************************
}
bool Patch::isConvex() {
    glm::vec3 preCross = glm::vec3(0.f);
    for (int i = 0;i < sides.size();i++) {
        int index1 = i;
        int index2 = (i + 1) % sides.size();
        glm::vec3 v1 = sides[index1].first->pos - sides[index1].second->pos;
        glm::vec3 v2 = sides[index2].first->pos - sides[index2].second->pos;
        //The cross product of all consecutive sides should fall in the same side 
        //if they form a convex shape, so the dot(crossProduct,crossProduct) should be positive
        glm::vec3 crossProduct = glm::cross(v1, v2);
        bool isConvex = glm::dot(preCross, crossProduct) >= 0;
        preCross = crossProduct;
        if (!isConvex)return false;
    }
    return true;
}
bool Patch::processSYM(
    std::unordered_map<std::pair<Vertex*, Vertex*>, HalfEdge*, HEdgeHash>* map,
    HalfEdge* hEdge, Vertex* sV, Vertex* eV) 
{
    std::pair<Vertex*, Vertex*> symKey(eV, sV);
    std::unordered_map<std::pair<Vertex*, Vertex*>, HalfEdge*, HEdgeHash>::iterator itr = map->find(symKey);
    if (itr != map->end()) {
        itr->second->setSymEdge(hEdge);
        return true;
    }
    else {
        //wait for the symkey
        std::pair<Vertex*, Vertex*> key(sV, eV);
        (*map)[key] = hEdge;
        return false;
    }
};
void Patch::quadrangulate() {
    if (hasQuad)return;
    if (!isConvex())return;
    //A naive method
    //first we try to combine triangles into quadrangles as many as possible
    //but these quadrangles are not Quad yet
    std::vector<std::vector<Vertex*>> candidates;
    for (int i = 0;i < (verts.size()-2);i++) {
        if (i + 3 < verts.size()) {
            //we have a quadrangle
            std::vector<Vertex*> candidate;
            candidate.push_back(verts[0].get());
            candidate.push_back(verts[i+1].get());
            candidate.push_back(verts[i+2].get());
            candidate.push_back(verts[i+3].get());
            candidates.push_back(candidate);
            //since we skip one triangle
            i++;
        }
        else {
            //we have a triangle
            std::vector<Vertex*> candidate;
            candidate.push_back(verts[0].get());
            candidate.push_back(verts[i + 1].get());
            candidate.push_back(verts[i + 2].get());
            candidates.push_back(candidate);
        }
    }
    int lastCandidateID = candidates.size() - 1;
    if (lastCandidateID < 0)return;
    //if there remains a triangle, we find its central point
    //convert it to quadrangle, 
    //subdivide quadrangle that shares the same edge with the triangle into 2 new quads
    //in our method, there will only be one quadrangle being affected
    //I update candidates here to make sure all candidates are quadrangles
    if (candidates[lastCandidateID].size() == 3) {
        //find centre of the triangle
        std::vector<Vertex*>& trig = candidates[lastCandidateID];
        glm::vec3 centre =
            trig[0]->pos +
            trig[1]->pos +
            trig[2]->pos;
        centre /= 3.f;
        candidates.pop_back();
        uPtr<Vertex> v1 = mkU<Vertex>((trig[0]->pos + trig[1]->pos) / 2.f, nullptr);//this is the vertex on the shared edge
        uPtr<Vertex> v2 = mkU<Vertex>((trig[1]->pos + trig[2]->pos) / 2.f, nullptr);
        uPtr<Vertex> v3 = mkU<Vertex>((trig[0]->pos + trig[2]->pos) / 2.f, nullptr);
        //split affected quadrangle
        if (lastCandidateID >= 1) {
            std::vector<Vertex*>& splitQuad = candidates[lastCandidateID - 1];
            candidates.pop_back();
            uPtr<Vertex> v4 = mkU<Vertex>((splitQuad[1]->pos + splitQuad[2]->pos) / 2.f, nullptr);
            std::vector<Vertex*> newCandidate1;
            std::vector<Vertex*> newCandidate2;

            newCandidate1.push_back(splitQuad[0]);
            newCandidate1.push_back(splitQuad[1]);
            newCandidate1.push_back(v4.get());
            newCandidate1.push_back(v1.get());

            newCandidate2.push_back(v4.get());
            newCandidate2.push_back(splitQuad[2]);
            newCandidate2.push_back(splitQuad[3]);
            newCandidate2.push_back(v1.get());

            candidates.push_back(newCandidate1);
            candidates.push_back(newCandidate2);

            verts.push_back(std::move(v4));
        }
        //split triangle
        uPtr<Vertex> c = mkU<Vertex>(centre, nullptr);
        std::vector<Vertex*> q1;
        std::vector<Vertex*> q2;
        std::vector<Vertex*> q3;
        q1.push_back(c.get());
        q1.push_back(v1.get());
        q1.push_back(trig[1]);
        q1.push_back(v2.get());

        q2.push_back(c.get());
        q2.push_back(v2.get());
        q2.push_back(trig[2]);
        q2.push_back(v3.get());

        q3.push_back(c.get());
        q3.push_back(v3.get());
        q3.push_back(trig[0]);
        q3.push_back(v1.get());

        candidates.push_back(q1);
        candidates.push_back(q2);
        candidates.push_back(q3);

        verts.push_back(std::move(v1));
        verts.push_back(std::move(v2));
        verts.push_back(std::move(v3));
    }

    //if all candidates are quarangles, we create Quads for Patch
    std::unordered_map<std::pair<Vertex*, Vertex*>, HalfEdge*, HEdgeHash> SYMmap;
    for (std::vector<Vertex*>& candidate : candidates) {
        uPtr<Quad> quad;

        uPtr<HalfEdge> s1 = mkU<HalfEdge>(nullptr, nullptr, quad.get(), candidate[0]);
        uPtr<HalfEdge> s2 = mkU<HalfEdge>(nullptr, nullptr, quad.get(), candidate[1]);
        uPtr<HalfEdge> s3 = mkU<HalfEdge>(nullptr, nullptr, quad.get(), candidate[2]);
        uPtr<HalfEdge> s4 = mkU<HalfEdge>(nullptr, nullptr, quad.get(), candidate[3]);

        s1->setNextEdge(s2.get());
        s2->setNextEdge(s3.get());
        s3->setNextEdge(s4.get());
        s4->setNextEdge(s1.get());

        processSYM(&SYMmap, s1.get(), candidate[3], candidate[0]);
        processSYM(&SYMmap, s2.get(), candidate[0], candidate[1]);
        processSYM(&SYMmap, s3.get(), candidate[1], candidate[2]);
        processSYM(&SYMmap, s4.get(), candidate[2], candidate[3]);

        quads.push_back(std::move(quad));
        hEdges.push_back(std::move(s1));
        hEdges.push_back(std::move(s2));
        hEdges.push_back(std::move(s3));
        hEdges.push_back(std::move(s4));
    }
    //outer halfedge on the boundary
    //I skip that
}
//this method will return new generated quad after division
void Patch::subDivide(HalfEdge* edgeToDivide, int divideNum) {
    //find all affected quads
    std::vector<Quad*> quads;
    HalfEdge* curr = edgeToDivide;
    bool isLoop = false;
    while (curr != nullptr) {
        Quad* currQuad = edgeToDivide->getQuad();
        if (currQuad != nullptr) {
            quads.push_back(currQuad);
        }
        curr = curr->getNextHalfEdge()->getNextHalfEdge();
        curr = curr->getSymHalfEdge();
        if (curr == edgeToDivide) {
            isLoop = true;
            break;
        }
    }

    if (!isLoop) {
        curr = edgeToDivide->getSymHalfEdge();
        while (curr != nullptr) {
            Quad* currQuad = edgeToDivide->getQuad();
            if (currQuad != nullptr) {
                quads.push_back(currQuad);
            }
            curr = curr->getNextHalfEdge()->getNextHalfEdge();
            curr = curr->getSymHalfEdge();
        }
    }
}