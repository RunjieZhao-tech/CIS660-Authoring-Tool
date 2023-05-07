#include "halfedge.h"
#include<iostream>
int HalfEdge::count = 0;
int Face::count = 0;
int Vertex::count = 0;
Vertex::Vertex(glm::vec3 pos, HalfEdge* hEdge)
    :hEdge(hEdge),sharp(false),radius(0.05f),pos(pos),id(Vertex::count++)
{
    QVariant var(id);
    QListWidgetItem::setText(var.toString());
    if(hEdge!=nullptr){
        hEdge->setVert(this);
    }
    jointIds.push_back(-1);
    jointIds.push_back(-1);
    jointWeights.push_back(0);
    jointWeights.push_back(0);
};
Vertex::Vertex()
    :Vertex(glm::vec3(0,0,0),nullptr)
{};
void Vertex::unBindJoints(){
    jointIds.clear();
    jointWeights.clear();
    jointIds.push_back(-1);
    jointIds.push_back(-1);
    jointWeights.push_back(0);
    jointWeights.push_back(0);
}
bool Vertex::bindJoint(int id,float weight){
    int size = 2;
    //new element will be the first in the new vector
    if(jointIds.size()<size){
        //swap it with first element
        if(jointIds.size()==0){
            jointIds.push_back(id);
            jointWeights.push_back(weight);
        }else{
            int id0 = jointIds[0];
            float weight0 = jointWeights[0];
            jointIds[0] = id;
            jointWeights[0] = weight;
            jointIds.push_back(id0);
            jointWeights.push_back(weight0);
        }
        return true;
    }else{
        float minWeight = 2;
        int minId = -1;
        //find joint with lowest weight
        for(unsigned long i=0;i<jointIds.size();i++){
            if(minWeight>jointWeights[i]){
                minId = i;
                minWeight = jointWeights[i];
            }
        }
        //if new weight is lower than lowest weight ignore
        if(minWeight>weight){return false;}
        //else we assign new value to the first
        //replace the lowest weight with the original first element
        jointIds[minId] = jointIds[0];
        jointWeights[minId] = jointWeights[0];
        jointIds[0] = id;
        jointWeights[0] = weight;
        return true;
    }
}
void Vertex::setSharp(bool v){
    if(v==sharp)return;
    sharp = v;
    if(!v){
        QVariant var(id);
        QListWidgetItem::setText(var.toString());
    }else{
        QListWidgetItem::setText(QListWidgetItem::text()+"_sharp");
    }
}
bool Vertex::hitTest(glm::vec3 o, glm::vec3 d, HitInfo* info){
    d = glm::normalize(d);
    float A = 1.f;
    float B = 2*(d.x*(o.x - pos.x)+d.y*(o.y - pos.y)+d.z*(o.z - pos.z));
    float C = (o.x - pos.x)*(o.x - pos.x)+
              (o.y - pos.y)*(o.y - pos.y)+
              (o.z - pos.z)*(o.z - pos.z)- radius*radius;
    float sqr = B*B - 4*A*C;
    if(sqr<0){return false;}
    float t0 = (-B-std::sqrt(sqr))/(2*A);
    float t1 = (-B+std::sqrt(sqr))/(2*A);
    if(t0>=0){
        glm::vec3 hitpos = o + t0*d;
        info->collider = this;
        info->hitPos = hitpos;
        return true;
    }else if(t1>=0){
        glm::vec3 hitpos = o + t1*d;
        info->collider = this;
        info->hitPos = hitpos;
        return true;
    }
    return false;
};
std::vector<HalfEdge*> Vertex::getHalfEdges(){
    std::vector<HalfEdge*> res;
    HalfEdge* current = hEdge;
    do{
        res.push_back(current);
        current = current->nHEdge->sHEdge;
    }while(current!=hEdge);
    return res;
};//return all half edges point to it

HalfEdge::HalfEdge(HalfEdge* nHEdge, HalfEdge* sHEdge, Face* face, Vertex* vert)
    :nHEdge(nHEdge),sHEdge(sHEdge),face(face),vert(vert),sharp(false),radius(0.04f),id(HalfEdge::count++)
{
    QVariant var(id);
    QListWidgetItem::setText(var.toString());
    if(sHEdge!=nullptr){
       sHEdge->sHEdge = this;
    }
    if(face!=nullptr){
        face->hEdge = this;
    }
    if(vert!=nullptr){
        vert->hEdge = this;
    }
};
HalfEdge::HalfEdge()
    :HalfEdge(nullptr,nullptr,nullptr,nullptr)
{};
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
void HalfEdge::setNextEdge(HalfEdge* nEdge) {
    this->nHEdge = nEdge;
}
HalfEdge* HalfEdge::getNextHalfEdge() {
    return this->nHEdge;
}
HalfEdge* HalfEdge::getSymHalfEdge() {
    return this->sHEdge;
}
bool HalfEdge::hitTest(glm::vec3 o, glm::vec3 d, HitInfo* info){
    d = glm::normalize(d);
    std::pair<Vertex*,Vertex*> p = getVerts();
    glm::vec3 segment = p.first->pos - p.second->pos;
    float x = segment.x;
    float y = segment.y;
    float z = segment.z;
    //ignore the condition hit on the flat plane
    //I assume the point hit (p1) is (o + t*d)
    //the length of the cross product of (p1-p.first) and (p.second - p.first)
    //should be lenght(p1-p.first)*length(p.second - p.first)*sin
    //the distance between the p1 and the segment should be
    //lenght(p1-p.first)*sin, which equals radius
    //here, I assume p1 - p.first = v0 + v1*t
    glm::vec3 v0 = o - p.first->pos;
    glm::vec3 v1 = d;
    float a0 = v0.x;
    float b0 = v0.y;
    float c0 = v0.z;
    float a1 = v1.x;
    float b1 = v1.y;
    float c1 = v1.z;
    float A = c1*c1*x*x - 2*x*z*c1*a1 + a1*a1*z*z +
              b1*b1*x*x - 2*x*y*b1*a1 + a1*a1*y*y +
              c1*c1*y*y - 2*y*z*c1*b1 + b1*b1*z*z;
    float B = 2*c0*c1*x*x - 2*x*z*(c0*a1 + c1*a0) + 2*a0*a1*z*z +
              2*b0*b1*x*x - 2*x*y*(b0*a1 + b1*a0) + 2*a0*a1*y*y +
              2*c0*c1*y*y - 2*y*z*(c0*b1 + c1*b0) + 2*b0*b1*z*z;
    float C = x*x*c0*c0 - 2*x*z*c0*a0 + z*z*a0*a0 +
              x*x*b0*b0 - 2*x*y*b0*a0 + y*y*a0*a0 +
              y*y*c0*c0 - 2*y*z*c0*b0 + z*z*b0*b0 -
              radius*radius*(x*x + y*y + z*z);
    float sqr = B*B - 4*A*C;
    if(sqr<0){return false;}
    float sqrt = std::sqrt(sqr);
    float t0 = (-B-sqrt)/(2*A);
    float t1 = (-B+sqrt)/(2*A);
    //candidate point
    glm::vec3 hitpos;
    if(t0>=0){
        hitpos = o + t0*d;
        bool isInSegment = glm::dot(hitpos - p.first->pos,p.second->pos- p.first->pos) >=0 &&
                           glm::dot(hitpos - p.second->pos,p.first->pos- p.second->pos) >=0;
        if(isInSegment){
            info->collider = this;
            info->hitPos = hitpos;
            return true;
        }
    }
    if(t1>=0){
        hitpos = o + t1*d;
        bool isInSegment = glm::dot(hitpos - p.first->pos,p.second->pos- p.first->pos) >=0 &&
                           glm::dot(hitpos - p.second->pos,p.first->pos- p.second->pos) >=0;
        if(isInSegment){
            info->collider = this;
            info->hitPos = hitpos;
            return true;
        }
    }
    return false;
};
void HalfEdge::setSharp(bool v){
    if(v==sharp)return;
    sharp = v;
    if(!v){
        QVariant var(id);
        QListWidgetItem::setText(var.toString());
    }else{
        QListWidgetItem::setText(QListWidgetItem::text()+"_sharp");
    }
    this->sHEdge->setSharp(v);
}
Face* HalfEdge::getFace(){
    return face;
};
std::pair<Vertex*,Vertex*> HalfEdge::getVerts(){
    return std::pair<Vertex*,Vertex*>(sHEdge->vert,vert);
};

Face::Face(HalfEdge * hEdge, glm::vec3 color)
    :hEdge(hEdge),sharp(false),color(color),id(Face::count++)
{
    QVariant var(id);
    QListWidgetItem::setText(var.toString());
    if(hEdge!=nullptr){
        hEdge->setFace(this);
    }
};
Face::Face()
    :Face(nullptr,glm::vec3(rand()%100/100.f,rand()%100/100.f,rand()%100/100.f))
{};
void Face::triangulate(std::vector<Vertex*>* outVerts,std::vector<unsigned int>* outIds){
    unsigned int start = outVerts->size();
    std::vector<HalfEdge*> edges = getHalfEdges();
    for(HalfEdge* edge:edges){
        outVerts->push_back(edge->vert);
    }
    int size = edges.size()-2;
    for(int i =0;i<size;i++){
        outIds->push_back(start+0);
        outIds->push_back(start+i+1);
        outIds->push_back(start+i+2);
    }
};
bool inTriangle(glm::vec3 testPt,glm::vec3 trigPt0,glm::vec3 trigPt1,glm::vec3 trigPt2,glm::vec3 norm){
    float s1 = abs(glm::dot(glm::cross(testPt-trigPt0,testPt-trigPt1),norm));
    float s2 = abs(glm::dot(glm::cross(testPt-trigPt1,testPt-trigPt2),norm));
    float s3 = abs(glm::dot(glm::cross(testPt-trigPt2,testPt-trigPt0),norm));
    float s  = abs(glm::dot(glm::cross(trigPt2-trigPt0,trigPt2-trigPt1),norm));
    return s>=(s1+s2+s3);
}
bool Face::hitTest(glm::vec3 origin, glm::vec3 dirc, HitInfo* info){
    std::vector<Vertex*> verts;
    std::vector<unsigned int> ids;
    triangulate(&verts,&ids);
    //could cause problem if the first three verts are in same line
    glm::vec3 norm = glm::cross(verts[0]->pos-verts[1]->pos,verts[0]->pos-verts[2]->pos);
    float t = glm::dot(norm,verts[0]->pos-origin)/glm::dot(norm,dirc);
    glm::vec3 planeHit = origin + t*dirc;
    bool hit = false;
    norm = glm::normalize(norm);
    unsigned long size = ids.size()/3;
    for(unsigned long i=0;i<size;i++){
        int id0 = ids[i*3];
        int id1 = ids[i*3+1];
        int id2 = ids[i*3+2];
        hit = hit || inTriangle(planeHit,verts[id0]->pos,verts[id1]->pos,verts[id2]->pos,norm);
    }
    if(hit){
        info->collider=this;
        info->hitPos=planeHit;
    }
    return hit;
};
void Face::setSharp(bool v){
    if(v==sharp)return;
    sharp = v;
    if(!v){
        QVariant var(id);
        QListWidgetItem::setText(var.toString());
    }else{
        QListWidgetItem::setText(QListWidgetItem::text()+"_sharp");
    }
    std::vector<HalfEdge*> edges = getHalfEdges();
    for(HalfEdge* edge: edges){
        edge->setSharp(v);
        edge->vert->setSharp(v);
    }
}
std::vector<Vertex*> Face::getVertices(){
    std::vector<Vertex*> verts;
    std::vector<HalfEdge*> edges = getHalfEdges();
    for(HalfEdge* edge:edges){
        verts.push_back(edge->vert);
    }
    return verts;
};
glm::vec3 Face::getCenter(){
    std::vector<Vertex*> verts = getVertices();
    unsigned long count = verts.size();
    glm::vec3 res(0,0,0);
    for(unsigned long i=0;i<verts.size();i++){
        res += verts[i]->pos;
    }
    res = 1.f/count*res;
    return res;
};
std::vector<HalfEdge*> Face::getHalfEdges(){
    std::vector<HalfEdge*> res;
    HalfEdge* current = hEdge;
    do{
        res.push_back(current);
        current = current->nHEdge;
    }while(current!=hEdge);
    return res;
};//return all half edges around it

void Face::setEdge(HalfEdge* e) {
    this->hEdge = e;
}
std::vector<Face*> Face::getAdjacentFaces(HalfEdge* halfEdge) {
    std::vector<Face*> result;
    HalfEdge* curr = halfEdge;
    do {
        if (curr->sHEdge != nullptr) {
            result.push_back(curr->sHEdge->face);
        }
        else {
            result.push_back(nullptr);
        }
        curr = curr->nHEdge;
    } while (curr != halfEdge);
    return result;
}

Patch::Patch()
    :hasQuad(false)
{
    std::vector<glm::vec3> vertices;
    glm::vec3 v = glm::vec3(0);
    vertices.push_back(v);

    v = glm::vec3(0,1,0);
    vertices.push_back(v);
    v = glm::vec3(1,1,0);
    vertices.push_back(v);
    v = glm::vec3(1,0,0);
    vertices.push_back(v);
//    v = glm::vec3(0.5,0,0);
//    vertices.push_back(v);

//    v = glm::vec3(1,-1,0);
//    vertices.push_back(v);
//    v = glm::vec3(2,0,0);
//    vertices.push_back(v);
//    v = glm::vec3(2,1,0);
//    vertices.push_back(v);
//    v = glm::vec3(1,2,0);
//    vertices.push_back(v);
//    v = glm::vec3(0,1,0);
//    vertices.push_back(v);


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
    std::vector<Vertex*> boundVerts;
    for(int i=0;i<verts.size();i++){
        boundVerts.push_back(verts[i].get());
    }
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
        else if(i + 2 < verts.size()){
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
        std::vector<Vertex*> trig = candidates[lastCandidateID];
        glm::vec3 centre =
            trig[0]->pos +
            trig[1]->pos +
            trig[2]->pos;
        centre /= 3.f;
        uPtr<Vertex> v1 = mkU<Vertex>((trig[0]->pos + trig[1]->pos) / 2.f, nullptr);//this is the vertex on the shared edge
        uPtr<Vertex> v2 = mkU<Vertex>((trig[1]->pos + trig[2]->pos) / 2.f, nullptr);
        uPtr<Vertex> v3 = mkU<Vertex>((trig[2]->pos + trig[0]->pos) / 2.f, nullptr);
        //add new vertex to boundary vertices
        for(int i=0;i<boundVerts.size();i++){
            if(boundVerts[i] == trig[1]){
                boundVerts.insert(boundVerts.begin()+i+1,v2.get());
            }
        }
        for(int i=0;i<boundVerts.size();i++){
            if(boundVerts[i] == trig[2]){
                boundVerts.insert(boundVerts.begin()+i+1,v3.get());
            }
        }
        //--------add new vertex to boundary vertices
        candidates.pop_back();
        //split affected quadrangle
        if (lastCandidateID >= 1) {
            std::vector<Vertex*> splitQuad = candidates[lastCandidateID - 1];

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

            candidates.pop_back();
            candidates.push_back(newCandidate1);
            candidates.push_back(newCandidate2);

            //add new vertex to boundary vertices
            for(int i=0;i<boundVerts.size();i++){
                if(boundVerts[i] == splitQuad[1]){
                    boundVerts.insert(boundVerts.begin()+i+1,v4.get());
                }
            }
            //--------add new vertex to boundary vertices
            verts.push_back(std::move(v4));

        }else{
            //add new vertex to boundary vertices
            for(int i=0;i<boundVerts.size();i++){
                if(boundVerts[i] == trig[0]){
                    boundVerts.insert(boundVerts.begin()+i+1,v1.get());
                }
            }
            //--------add new vertex to boundary vertices
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

        verts.push_back(std::move(c));
        verts.push_back(std::move(v1));
        verts.push_back(std::move(v2));
        verts.push_back(std::move(v3));
    }

    //if all candidates are quarangles, we create Quads for Patch
    std::unordered_map<std::pair<Vertex*, Vertex*>, HalfEdge*, HEdgeHash> SYMmap;
    for (std::vector<Vertex*>& candidate : candidates) {
        uPtr<Face> quad = mkU<Face>();

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
    for(int i=0;i<boundVerts.size();i++){
        uPtr<HalfEdge> b = mkU<HalfEdge>(nullptr, nullptr, nullptr, boundVerts[i]);
        hEdges.push_back(std::move(b));
    }
    for(int i=0;i<boundVerts.size();i++){
        int index1 = i;
        int index2 = (i+1)%boundVerts.size();
        int index3 = (i+2)%boundVerts.size();

        //since it's sym half edge on the boundary the order should be inverse
        HalfEdge* nextH = boundVerts[index1]->hEdge;
        HalfEdge* h = boundVerts[index2]->hEdge;

        h->setNextEdge(nextH);
        bool success= processSYM(&SYMmap, h, boundVerts[index3], boundVerts[index2]);
    }

    //find bound to subdivide
    auto edges2Search = boundVerts[0]->getHalfEdges();
    HalfEdge* firstBound = nullptr;
    for (int i = 0;i < edges2Search.size();i++) {
        firstBound = edges2Search[i];
        if (firstBound->getFace() == nullptr)break;
    }
    std::vector<HalfEdge*> edges2Subdivide;
    for (int i = 0;i < boundVerts.size() / 2;i++) {
        edges2Subdivide.push_back(firstBound);
        firstBound = firstBound->getNextHalfEdge();
    }
    for (auto bound : edges2Subdivide) {
        subDivide(bound, 5);
    }
    hasQuad = true;
}
//this method will return new generated quad after division
void Patch::subDivide(HalfEdge* edgeToDivide, int divideNum) {
    bool isOnBoundary = edgeToDivide->getFace() == nullptr;
    HalfEdge* start = edgeToDivide;//find a boundary edge to start
    //to make sure it will not be a loop
    bool isLoop = false;
    if (!isOnBoundary) {
        do {
            start = start->getSymHalfEdge();
            isOnBoundary = start->getFace() == nullptr;
            if (isOnBoundary) {
                break;
            }
            start = start->getNextHalfEdge()->getNextHalfEdge();
            if (start == edgeToDivide) {
                isLoop = true;
                break;
            }
        } while (start != edgeToDivide);
    }
    std::cout << "id: " << start->id << std::endl;
    std::cout << (start->getSymHalfEdge()->getFace() != nullptr) << std::endl;
    if (isLoop) {
        std::cout << "loop!" << std::endl;
        return;
    }

    //cut boundary into subverts, exclude 2 verts we already have
    std::vector<Vertex*> subVerts;
    for (int i = 0;i < divideNum-1;i++) {
        std::pair<Vertex*, Vertex*> p = start->getVerts();
        glm::vec3 mixPos = glm::mix(p.first->pos, p.second->pos, (i + 1.f) / divideNum);
        uPtr<Vertex> subVert = mkU<Vertex>(mixPos, nullptr);
        subVerts.push_back(subVert.get());
        verts.push_back(std::move(subVert));
    }
    //process the boundary
    HalfEdge* edgeToSetNext = start->getNextHalfEdge();
    do {
        edgeToSetNext = edgeToSetNext->getNextHalfEdge();
    } while (edgeToSetNext->getNextHalfEdge() != start);
    std::vector<HalfEdge*> setNextHalf;
    setNextHalf.push_back(edgeToSetNext);
    for (int i = 0;i < subVerts.size();i++) {
        uPtr<HalfEdge> hE = mkU<HalfEdge>(nullptr, nullptr, nullptr, subVerts[i]);
        setNextHalf.push_back(hE.get());
        hEdges.push_back(std::move(hE));
    }
    setNextHalf.push_back(start);
    for (int i = 0;i < setNextHalf.size() - 1;i++) {
        setNextHalf[i]->setNextEdge(setNextHalf[i + 1]);
    }

    //process quad between boundary
    HalfEdge* curr = start;
    while (curr->getSymHalfEdge()->getFace() != nullptr) {
        std::vector<Vertex*> nextVerts;
        curr = subDivideQuad(subVerts, curr, nextVerts);
        subVerts = nextVerts;
        std::cout << "process quad" << std::endl;
    }

    //process boundary again
    start = curr->getSymHalfEdge();
    edgeToSetNext = start->getNextHalfEdge();
    do {
        edgeToSetNext = edgeToSetNext->getNextHalfEdge();
    } while (edgeToSetNext->getNextHalfEdge() != start);
    std::vector<HalfEdge*> nextHalf;
    nextHalf.push_back(edgeToSetNext);
    std::vector<HalfEdge*> symHalf;
    symHalf.push_back(curr);
    for (int i = subVerts.size() - 1;i >= 0;i--) {
        //assumption1: since the new generated halfedge will always make vertex update its halfedge
        symHalf.push_back(subVerts[i]->hEdge);
        uPtr<HalfEdge> hE = mkU<HalfEdge>(nullptr, nullptr, nullptr, subVerts[i]);
        nextHalf.push_back(hE.get());
        hEdges.push_back(std::move(hE));
    }
    nextHalf.push_back(start);
    for (int i = 0;i < nextHalf.size() - 1;i++) {
        nextHalf[i]->setNextEdge(nextHalf[i + 1]);

    }
    for (int i = 0;i < symHalf.size();i++) {
        nextHalf[i + 1]->setSymEdge(symHalf[i]);
    }
}

HalfEdge* Patch::subDivideQuad
    (std::vector<Vertex*>& newVerts, HalfEdge* adjacentHalfEdge,  std::vector<Vertex*>& out_newVerts) {
    //std::vector<Vertex*> out_oppositeHalfEdge;
    HalfEdge* out_oppositeHalfEdge = adjacentHalfEdge;
    if (newVerts.size() == 0)return out_oppositeHalfEdge;
    HalfEdge* halfEdge = adjacentHalfEdge->getSymHalfEdge();
    //store symEdge from the quad that inputs adjacentHalfEdge
    std::vector<HalfEdge*> symEdge;
    for (int i = 0;i < newVerts.size();i++) {
        //assumption1: since the new generated halfedge will always make vertex update its halfedge
        symEdge.push_back(newVerts[i]->hEdge);
    }
    symEdge.push_back(adjacentHalfEdge);

    //store the halfedges that shares the side with quad that inputs halfedge
    std::vector<HalfEdge*> hEdge;
    for (int i = newVerts.size()-1;i >=0 ;i--) {
        uPtr<HalfEdge> newHEdge = mkU<HalfEdge>(nullptr, nullptr, nullptr, newVerts[i]);
        hEdge.push_back(newHEdge.get());
        hEdges.push_back(std::move(newHEdge));
    }
    hEdge.push_back(halfEdge);

    //calculate new Vertex for output
    std::pair<Vertex*, Vertex*> pair = halfEdge->getNextHalfEdge()->getNextHalfEdge()->getVerts();
    for (int i = 0;i < newVerts.size();i++) {
        glm::vec3 mixPos = glm::mix(pair.first->pos, pair.second->pos, (i + 1.f) / (newVerts.size()+1));
        uPtr<Vertex> newVert = mkU<Vertex>(mixPos, nullptr);
        out_newVerts.push_back(newVert.get());
        verts.push_back(std::move(newVert));
    }

    //generate halfedges that are going to share side with next adjacent quad
    std::vector<HalfEdge*> toCombineEdge;
    for (int i = 0;i < out_newVerts.size();i++) {
        uPtr<HalfEdge> newHEdge = mkU<HalfEdge>(nullptr, nullptr, nullptr, out_newVerts[i]);
        toCombineEdge.push_back(newHEdge.get());
        hEdges.push_back(std::move(newHEdge));
    }
    out_oppositeHalfEdge = halfEdge->getNextHalfEdge()->getNextHalfEdge();
    toCombineEdge.push_back(out_oppositeHalfEdge);

    //generate new quads
    std::vector<Face*> allQuads;
    allQuads.push_back(halfEdge->getFace());
    for (int i = 0;i < out_newVerts.size();i++) {
        uPtr<Face> newQuad = mkU<Face>();
        allQuads.push_back(newQuad.get());
        quads.push_back(std::move(newQuad));
    }

    //connect hEdge and toCombineEdge
    //   ^<----e1-----^
    //   ||           |out_oppositeHalfEdge
    //   ||-----------|
    //   ||           |
    //   ||-----------|
    //   ||halfEdge   |
    //    v----e2----->

    std::unordered_map<std::pair<Vertex*, Vertex*>, HalfEdge*, HEdgeHash> SYMmap;
    //for halfedge in the center
    for (int i = 0;i < out_newVerts.size()-1;i++) {
        HalfEdge* rightE = toCombineEdge[i + 1];
        HalfEdge* leftE = hEdge[hEdge.size() - 1 - (i + 1)];

        uPtr<HalfEdge> lowerE = mkU<HalfEdge>(rightE, nullptr,
                            allQuads[i + 1], nullptr);
        //must do this otherwise the newverts's halfedge will be reset which will affect our assumption1
        lowerE->vert = out_newVerts[i];
        processSYM(&SYMmap, lowerE.get(), newVerts[i], out_newVerts[i]);

        uPtr<HalfEdge> upperE = mkU<HalfEdge>(leftE, nullptr,
                            allQuads[i + 1], nullptr);
        upperE->vert = newVerts[i + 1];
        processSYM(&SYMmap, upperE.get(), out_newVerts[i + 1], newVerts[i + 1]);

        rightE->setNextEdge(upperE.get());
        leftE->setNextEdge(lowerE.get());
        rightE->setFace(allQuads[i + 1]);
        leftE->setFace(allQuads[i + 1]);

        hEdges.push_back(std::move(lowerE));
        hEdges.push_back(std::move(upperE));
    }
    //for halfedge on the upper and lower bound

    //For upper bound
    //upper edge
    HalfEdge* e1 = out_oppositeHalfEdge->getNextHalfEdge();
    e1->setNextEdge(hEdge[0]);
    e1->setFace(allQuads[allQuads.size() - 1]);
    //right edge
    out_oppositeHalfEdge->setFace(allQuads[allQuads.size() - 1]);

    //lower edge
    uPtr<HalfEdge> upperLowerBound = mkU<HalfEdge>(out_oppositeHalfEdge, nullptr,
        allQuads[allQuads.size() - 1],nullptr);
    upperLowerBound->vert =  out_newVerts[out_newVerts.size() - 1];
    processSYM(&SYMmap, upperLowerBound.get(), newVerts[out_newVerts.size() - 1], out_newVerts[out_newVerts.size() - 1]);
    //left edge
    hEdge[0]->setFace(allQuads[allQuads.size() - 1]);
    hEdge[0]->setNextEdge(upperLowerBound.get());

    hEdges.push_back(std::move(upperLowerBound));
    //lower bound
    //lower
    HalfEdge* e2 = halfEdge->getNextHalfEdge();
    e2->setNextEdge(toCombineEdge[0]);
    //upper
    uPtr<HalfEdge> lowerUpperBound = mkU<HalfEdge>(halfEdge, nullptr,
        allQuads[0], nullptr);
    lowerUpperBound->vert = newVerts[0];
    processSYM(&SYMmap, lowerUpperBound.get(), out_newVerts[0], newVerts[0]);
    //left
    //we don't need to do anything
    //right
    toCombineEdge[0]->setNextEdge(lowerUpperBound.get());
    toCombineEdge[0]->setFace(allQuads[0]);
    hEdges.push_back(std::move(lowerUpperBound));

    //handle left symmetry
    for (int i = 0;i < symEdge.size();i++) {
        symEdge[i]->setSymEdge(hEdge[symEdge.size() - 1 - i]);
    }

    return out_oppositeHalfEdge;
}
