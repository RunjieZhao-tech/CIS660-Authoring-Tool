#include "mesh.h"
#include <QTextStream>
#include <QFile>
#include <iostream>
#include "filehelper.h"
#include <unordered_map>
#include <unordered_set>
Mesh::Mesh(OpenGLContext* mp_context)
    :Drawable(mp_context),m_sharpness(0.f)
{}

void Mesh::create(){
    std::vector<GLuint>    glIndices;
    std::vector<glm::vec4> glPos;
    std::vector<glm::vec4> glCol;
    std::vector<glm::vec4> glNor;
    std::vector<int>       glJointIds;
    std::vector<float>     glWeights;

    for(unsigned long i=0;i<faces.size();i++){
        Face* f = faces[i].get();
        triangulate(f,&glIndices,&glPos,&glCol,&glNor,
                    &glJointIds,&glWeights);
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
void Mesh::cloneDraw(std::vector<GLuint>* outIds, std::vector<glm::vec4>* outPos,
               std::vector<glm::vec4>* outCol, std::vector<glm::vec4>* outNor,
               std::vector<int>* outJIds,std::vector<float>* outWeights)const{
    for(unsigned long i=0;i<faces.size();i++){
        Face* f = faces[i].get();
        triangulate(f,outIds,outPos,outCol,outNor,outJIds,outWeights);
    }
}


void Mesh::triangulate(Face* face,
                       std::vector<GLuint>*    outIds,
                       std::vector<glm::vec4>* outPos,
                       std::vector<glm::vec4>* outCol,
                       std::vector<glm::vec4>* outNor,
                       std::vector<int>*       outJIds,
                       std::vector<float>*     outWeights)const{
    int ids = outPos->size();
    std::vector<HalfEdge*> edges = face->getHalfEdges();
    int size = edges.size()-2;
    for(int i =0;i<size;i++){
        Vertex* v0 = edges[0]->vert;
        Vertex* v1 = edges[i+1]->vert;
        Vertex* v2 = edges[i+2]->vert;
        Vertex* v[3] ={v0,v1,v2};
        glm::vec4 norm = glm::vec4(glm::cross(v1->pos-v0->pos,v2->pos-v1->pos),0);

        for(int j=0;j<3;j++){
           Vertex* vert = v[j];
           outPos->push_back(glm::vec4(vert->pos,1));
           outCol->push_back(glm::vec4(face->color,1));
           outNor->push_back(norm);
           outIds->push_back(ids++);
           for(unsigned long k=0;k<vert->jointIds.size();k++){
               outJIds->push_back(vert->jointIds[k]);
               outWeights->push_back(vert->jointWeights[k]);
           }
        }
    }
}

void Mesh::createFromOBJ(const char* filename){
    this->destroy();
    faces.clear();
    hEdges.clear();
    verts.clear();
    std::vector<glm::vec3> vs;
    std::vector<std::vector<int>> fs;
    readFile(filename,&vs,&fs);
    //end read position and ids of positions in faces
    for(unsigned long i=0;i<vs.size();i++){
        //*****************
        //verts modified here
        //*****************
        verts.push_back(mkU<Vertex>(vs[i],nullptr));
    }
    std::unordered_map<std::pair<Vertex*,Vertex*>,HalfEdge*,HEdgeHash> map;
    for(unsigned long i=0;i<fs.size();i++){ 
        std::vector<int> face = fs[i];
        uPtr<Face> uptrFace = mkU<Face>();
        std::vector<HalfEdge*> edgeAddr;
        //create unique pointer for halfedges
        for(unsigned long j=1;j<=face.size();j++){
            int sVertId;
            int eVertId;
            if(j<face.size()){
                sVertId = face[j-1];
                eVertId = face[j];
            }else{
                sVertId = face[j-1];
                eVertId = face[0];
            }
            //HalfEdge(HalfEdge* nHEdge, HalfEdge* sHEdge, Face* face, Vertex* vert);
            uPtr<HalfEdge> edge = mkU<HalfEdge>(nullptr,nullptr,uptrFace.get(),verts[eVertId].get());
            //set sHEdge
            processSYM(&map,edge.get(),verts[eVertId].get(),verts[sVertId].get());
            //end set sHEdge
            edgeAddr.push_back(edge.get());
            //*****************
            //hEdges modified here
            //*****************
            hEdges.push_back(move(edge));
        }
        //set nHEdge
        for(unsigned long j=0;j<edgeAddr.size();j++){
            if(j==(edgeAddr.size()-1)){
                edgeAddr[j]->nHEdge = edgeAddr[0];
            }else{
                edgeAddr[j]->nHEdge = edgeAddr[j+1];
            }
        }
        //end set nHEdge
        //end halfedges
        //*****************
        //faces modified here
        //*****************
        faces.push_back(move(uptrFace));
    }
    this->create();
};

void Mesh::addMidPoint(HalfEdge* halfedge, HalfEdge* &newHalfEdge,
                       HalfEdge* &newSymEdge, Vertex* &newVert){
    //create new two halfedge and the new vertex
    HalfEdge* SYM = halfedge->sHEdge;
    std::pair<Vertex*,Vertex*> vertp = halfedge->getVerts();
    //***************************************************
    //crash if I set face's halfedge to new halfedges??
    //somehow works now
    uPtr<HalfEdge> newHEdge = mkU<HalfEdge>(halfedge->nHEdge,SYM,halfedge->face,halfedge->vert);
    uPtr<HalfEdge> newSYM = mkU<HalfEdge>(SYM->nHEdge,halfedge,SYM->face,SYM->vert);

    uPtr<Vertex> newVertex = mkU<Vertex>();
    newVertex->pos = 0.5f*(vertp.first->pos + vertp.second->pos);
    halfedge->setVert(newVertex.get());
    SYM->setVert(newVertex.get());

    halfedge->nHEdge = newHEdge.get();
    SYM->nHEdge = newSYM.get();

    //fill the outputs
    newHalfEdge = newHEdge.get();
    newSymEdge = newSYM.get();
    newVert = newVertex.get();

    //assign them to the vector
    hEdges.push_back(std::move(newHEdge));
    hEdges.push_back(std::move(newSYM));
    verts.push_back(std::move(newVertex));

    //modify model
    destroy();
    create();
};

void Mesh::splitFace(Face* face, std::vector<Face*> *out_newFaces, std::vector<HalfEdge*>* out_newHalfEdges){
    std::vector<HalfEdge*> edges = face->getHalfEdges();
    std::vector<uPtr<HalfEdge>> newHalfEdges;
    std::vector<uPtr<Face>> newFaces;
    int num = edges.size()-2;//number of triangles
    if(num==1)return;
    //therefore there will be at least 2 faces
    HalfEdge* SYM = nullptr;//used to store symmetric half edge
    Vertex* origin = edges[0]->getVerts().first;
    for(int i=0;i<num;i++){
        HalfEdge* firstEdge = nullptr;//frist edge of the triangle
        HalfEdge* secondEdge = edges[i+1];
        HalfEdge* thirdEdge = nullptr;
        Face* currentFace = nullptr;

        //create first edge of the triangle
        if(i==0){
           //we will reuse the edge of the original face,
           //and reuse the original face
           firstEdge = edges[0];
           currentFace = face;
        }else{
            //we create new edge and face
            uPtr<Face> newFace = mkU<Face>();
            currentFace = newFace.get();
            std::pair<Vertex*,Vertex*> verts = secondEdge->getVerts();
            uPtr<HalfEdge> halfEdge = mkU<HalfEdge>(nullptr,SYM,nullptr,verts.first);
            firstEdge = halfEdge.get();
            newHalfEdges.push_back(std::move(halfEdge));
            newFaces.push_back(std::move(newFace));
        }
        //create third edge of the triangle
        if(i==num-1){
            //we reuse the original edge
            thirdEdge = secondEdge->nHEdge;
        }else{
            //we create new edge
            uPtr<HalfEdge> halfEdge1 = mkU<HalfEdge>(nullptr,nullptr,nullptr,origin);
            SYM = halfEdge1.get();
            thirdEdge = halfEdge1.get();
            newHalfEdges.push_back(std::move(halfEdge1));
        }
        firstEdge->nHEdge = secondEdge;
        secondEdge->nHEdge = thirdEdge;
        thirdEdge->nHEdge = firstEdge;
        firstEdge->setFace(currentFace);
        secondEdge->setFace(currentFace);
        thirdEdge->setFace(currentFace);
    }
    //fill the outputs
    //assign them to the vector
    for(uPtr<HalfEdge>&item:newHalfEdges){
        out_newHalfEdges->push_back(item.get());
        hEdges.push_back(std::move(item));
    }
    for(uPtr<Face>&item:newFaces){
        out_newFaces->push_back(item.get());
        faces.push_back(std::move(item));
    }

    //modify model
    destroy();
    create();
};

void Mesh::subDivide(HalfEdge* halfedge,const std::vector<glm::vec3>* centers){
    //create new two halfedge and the new vertex
    HalfEdge* SYM = halfedge->sHEdge;
    std::pair<Vertex*,Vertex*> vertp = halfedge->getVerts();
    //***************************************************8
    //crash if I set face's halfedge to new halfedges??
    uPtr<HalfEdge> newHEdge = mkU<HalfEdge>(halfedge->nHEdge,SYM,nullptr,halfedge->vert);
    uPtr<HalfEdge> newSYM = mkU<HalfEdge>(SYM->nHEdge,halfedge,nullptr,SYM->vert);
    newHEdge->setFace(halfedge->face);
    newSYM->setFace(SYM->face);

    //set vertex
    uPtr<Vertex> newVertex = mkU<Vertex>();
    glm::vec3 vertPos(0,0,0);
    //modified for sharp
    if(halfedge->sharp){
        int count = 2;
        vertPos += vertp.first->pos;
        vertPos += vertp.second->pos;
        vertPos = 1.f/count * vertPos;
    }else{
        float count = 2 + (*centers).size();
        for(glm::vec3 center:*centers){
            vertPos += center;
        }
        vertPos += vertp.first->pos;
        vertPos += vertp.second->pos;
        vertPos =  vertPos/count;
        //modify for semisharp
        glm::vec3 sharpPos = (vertp.first->pos + vertp.second->pos)/2.f;
        vertPos = vertPos*(1-m_sharpness) + sharpPos*m_sharpness;
    }
    newVertex->pos = vertPos;
    halfedge->setVert(newVertex.get());
    SYM->setVert(newVertex.get());

    halfedge->nHEdge = newHEdge.get();
    SYM->nHEdge = newSYM.get();
    //modified for sharp
    if(halfedge->sharp){
        newHEdge->setSharp(true);
        newSYM->setSharp(true);
    }

    //assign them to the vector
    hEdges.push_back(std::move(newHEdge));
    hEdges.push_back(std::move(newSYM));
    verts.push_back(std::move(newVertex));
};

void Mesh::quadrangulate(Face* face,const glm::vec3 center,std::vector<Vertex*>* midPts){
    Face* currFace = face;
    //modify for sharp
    std::vector<Face*> newFaces;
    //set currHalfEdge to the halfedge that starts at a mid point
    //and points to a non-mid point
    uPtr<Vertex> newVert = mkU<Vertex>();
    newVert->pos = center;
    HalfEdge* currHalfEdge = face->hEdge;
    bool pointToMid = false;
    for(Vertex* vert:*midPts){
        if(vert==currHalfEdge->vert){
            pointToMid = true;
        }
    }
    if(pointToMid){
        currHalfEdge = currHalfEdge->nHEdge;
    }
    HalfEdge* startHalfEdge = currHalfEdge;
    //used for set symmetric halfedge
    std::unordered_map<std::pair<Vertex*,Vertex*>,HalfEdge*,HEdgeHash> symMap;
    int count = 0;
    do{
        HalfEdge* nextEdge = currHalfEdge->nHEdge->nHEdge;
        //first edge of quadrangle
        uPtr<HalfEdge> firstEdge = mkU<HalfEdge>(nullptr,nullptr,nullptr,currHalfEdge->sHEdge->vert);
        processSYM(&symMap,firstEdge.get(),newVert.get(),firstEdge->vert);
        //second edge of quadrangle is currHalfEdge
        HalfEdge* secondEdge = currHalfEdge;
        //third edge of quadrangle
        HalfEdge* thridEdge = currHalfEdge->nHEdge;
        //fourth edge of quadrangle
        uPtr<HalfEdge> fourthEdge = mkU<HalfEdge>(nullptr,nullptr,nullptr,newVert.get());
        processSYM(&symMap,fourthEdge.get(),thridEdge->vert,newVert.get());

        firstEdge->setFace(currFace);
        thridEdge->setFace(currFace);
        fourthEdge->setFace(currFace);
        secondEdge->setFace(currFace);
        firstEdge->nHEdge = secondEdge;
        secondEdge->nHEdge = thridEdge;
        thridEdge->nHEdge = fourthEdge.get();
        fourthEdge->nHEdge = firstEdge.get();
        hEdges.push_back(std::move(firstEdge));
        hEdges.push_back(std::move(fourthEdge));
        currHalfEdge = nextEdge;

        if(currHalfEdge!=startHalfEdge){
            uPtr<Face> newFace = mkU<Face>(nullptr,currFace->color);
            currFace = newFace.get();
            faces.push_back(std::move(newFace));
            //modify for sharp
            newFaces.push_back(currFace);
        }
        if((++count)>100){
            std::cout<<"something wrong in quadrangulate"<<std::endl;
            break;
        }
    }while(currHalfEdge!=startHalfEdge);
    verts.push_back(std::move(newVert));
    //modify for sharp
    if(face->sharp){
        for(Face* newFace: newFaces){
            newFace->setSharp(true);
        }
    }
};

void Mesh::CCSubdivision(std::vector<Face*>* out_newFaces,std::vector<HalfEdge*>* out_newHalfEdges, std::vector<Vertex*>* out_newVerts){
    std::unordered_map<Face*,glm::vec3> centerMap;
    unsigned long vertsLen = verts.size();
    unsigned long facesLen = faces.size();
    unsigned long edgesLen = hEdges.size();
    //For each Face, compute its centroid
    for(uPtr<Face>&face:faces){
        centerMap[face.get()] = face->getCenter();
    }
    //Compute the smoothed midpoint of each edge in the mesh
    std::unordered_set<HalfEdge*> readyEdges;
    for(unsigned long i=0;i<edgesLen;i++){
        HalfEdge* currEdge = hEdges[i].get();
        bool alreadyDivide = readyEdges.find(currEdge)!=readyEdges.end();
        if(alreadyDivide){
            continue;
        }
        readyEdges.insert(currEdge->sHEdge);
        std::vector<glm::vec3> centers;
        centers.push_back(centerMap[currEdge->face]);
        centers.push_back(centerMap[currEdge->sHEdge->face]);
        subDivide(currEdge,&centers);
    }
    std::cout<<"finish subdivide midpoints"<< std::endl;
    //Smooth the original vertices
    for(unsigned long i=0;i<vertsLen;i++){
        Vertex* currVert = verts[i].get();
        if(currVert->sharp)continue;//modify for sharp
        int sharpCount = 0;//modify for sharp
        glm::vec3 sharpSum(0,0,0);//modify for sharp
        std::vector<HalfEdge*> edges = currVert->getHalfEdges();
        float n = edges.size()*1.f;
        glm::vec3 sumE(0,0,0);
        glm::vec3 sumF(0,0,0);
        for(HalfEdge* edge: edges){
            sumE += edge->sHEdge->vert->pos;
            sumF += centerMap[edge->face];
            //modify for sharp
            if(edge->sharp){
                sharpCount++;
                //because the current edge is only the half of
                //the edge, we need to go further
                sharpSum += edge->sHEdge->nHEdge->vert->pos;
            }
            //end modify for sharp
        }
        //modify for sharp
        if(sharpCount>=3)continue;
        if(sharpCount==2){
            currVert->pos = currVert->pos*0.75f + 0.125f*sharpSum;
            continue;
        }
        //end modify for sharp
        //modify semisharp
        glm::vec3 smoothPos = (n-2)/n*currVert->pos + 1/(n*n)*(sumE + sumF);
        currVert->pos = m_sharpness*currVert->pos + (1-m_sharpness)* smoothPos;
    }
    std::cout<<"finish smooth vertices"<<std::endl;
    //For each original face, split that face into N quadrangle faces
    for(unsigned long i=0;i<facesLen;i++){
        Face* currFace = faces[i].get();
        std::vector<Vertex*> midPts;
        std::vector<HalfEdge*> edges = currFace->getHalfEdges();
        for(unsigned long j=0;j<edges.size();j++){
            bool pointToMid = (j%2)==1;
            if(pointToMid){
                midPts.push_back(edges[j]->vert);
            }
        }
        quadrangulate(currFace,centerMap[currFace],&midPts);
    }
    std::cout<<"quadrangulate"<<std::endl;
    //assign outputs
    for(unsigned long i=facesLen;i<faces.size();i++){
        out_newFaces->push_back(faces[i].get());
    }
    for(unsigned long i=edgesLen;i<hEdges.size();i++){
        out_newHalfEdges->push_back(hEdges[i].get());
    }
    for(unsigned long i=vertsLen;i<verts.size();i++){
        out_newVerts->push_back(verts[i].get());
    }

    destroy();
    create();
};

void Mesh::extrudeFace(Face* face,float len,
                       std::vector<Face*>* out_newFaces,
                       std::vector<HalfEdge*>* out_newHalfEdges,
                       std::vector<Vertex*>* out_newVerts){
    std::vector<HalfEdge*> halfEdges = face->getHalfEdges();
    std::vector<HalfEdge*> clonedHalfEdges;
    std::vector<HalfEdge*> clonedSymEdges;
    std::vector<Face*> newFace;
    std::vector<Vertex*> newVerts;
    //take face out from mesh
    for(HalfEdge* halfEdges:halfEdges){
        //set the original edge not bind to face anymore
        halfEdges->face = nullptr;
        //duplicate vert and edges
        //Vertex(glm::vec3 pos, HalfEdge* hEdge);
        uPtr<Vertex> newVert = mkU<Vertex>(halfEdges->vert->pos,nullptr);
        //HalfEdge(HalfEdge* nHEdge, HalfEdge* sHEdge, Face* face, Vertex* vert);
        uPtr<HalfEdge> newHEdge = mkU<HalfEdge>(nullptr,nullptr,face,newVert.get());
        uPtr<HalfEdge> newSYM = mkU<HalfEdge>(nullptr,newHEdge.get(),nullptr,nullptr);
        //move to mesh vector and assign
        newVerts.push_back(newVert.get());
        clonedHalfEdges.push_back(newHEdge.get());
        clonedSymEdges.push_back(newSYM.get());

        verts.push_back(std::move(newVert));
        hEdges.push_back(std::move(newHEdge));
        hEdges.push_back(std::move(newSYM));
    }
    unsigned long size = clonedHalfEdges.size();
    glm::vec3 p0 = halfEdges[0]->vert->pos;
    glm::vec3 p1 = halfEdges[1]->vert->pos;
    glm::vec3 p2 = halfEdges[2]->vert->pos;
    glm::vec3 norm = glm::normalize(glm::cross(p1-p0,p2-p1));
    for(unsigned long i=0;i<size;i++){
        int nextI = i==(size-1)?0:(i+1);
        int preI = i==0?(size-1):(i-1);
        clonedHalfEdges[i]->nHEdge = clonedHalfEdges[nextI];
        clonedSymEdges[i]->setVert(newVerts[preI]);
        newVerts[i]->pos += norm*len;
    }
    //create new Faces
    std::unordered_map<std::pair<Vertex*,Vertex*>,HalfEdge*,HEdgeHash> symMap;
    for(unsigned long i=0;i<size;i++){
        uPtr<Face> newFace = mkU<Face>();
        std::pair<Vertex*,Vertex*> p1 = halfEdges[i]->getVerts();
        std::pair<Vertex*,Vertex*> p2 = clonedSymEdges[i]->getVerts();
        //set next edges
        uPtr<HalfEdge> hEdge1 = mkU<HalfEdge>(clonedSymEdges[i],nullptr,nullptr,p2.first);
        uPtr<HalfEdge> hEdge2 = mkU<HalfEdge>(halfEdges[i],nullptr,nullptr,p1.first);
        halfEdges[i]->nHEdge = hEdge1.get();
        clonedSymEdges[i]->nHEdge = hEdge2.get();
        //set faces
        hEdge1->setFace(newFace.get());
        hEdge2->setFace(newFace.get());
        halfEdges[i]->setFace(newFace.get());
        clonedSymEdges[i]->setFace(newFace.get());
        //set sym
        processSYM(&symMap,hEdge1.get(),halfEdges[i]->vert,hEdge1->vert);
        processSYM(&symMap,hEdge2.get(),clonedSymEdges[i]->vert,hEdge2->vert);

        out_newFaces->push_back(newFace.get());
        out_newHalfEdges->push_back(hEdge1.get());
        out_newHalfEdges->push_back(hEdge2.get());

        faces.push_back(std::move(newFace));
        hEdges.push_back(std::move(hEdge1));
        hEdges.push_back(std::move(hEdge2));
    }
    for(unsigned long i=0;i<size;i++){
        out_newHalfEdges->push_back(clonedSymEdges[i]);
        out_newHalfEdges->push_back(clonedHalfEdges[i]);
        out_newVerts->push_back(newVerts[i]);
    }
    destroy();
    create();
};

void Mesh::processSYM(std::unordered_map<std::pair<Vertex*,Vertex*>,HalfEdge*,HEdgeHash>*map,
                      HalfEdge* hEdge,Vertex* sV,Vertex* eV){
    std::pair<Vertex*,Vertex*> symKey(eV,sV);
    std::unordered_map<std::pair<Vertex*,Vertex*>,HalfEdge*,HEdgeHash>::iterator itr = map->find(symKey);
    if(itr!=map->end()){
        itr->second->setSymEdge(hEdge);
    }else{
        //wait for the symkey
        std::pair<Vertex*,Vertex*> key(sV,eV);
        (*map)[key] = hEdge;
    }
};
void Mesh::setSharpness(float input){
    m_sharpness = glm::clamp(input,0.f,1.f);
};
