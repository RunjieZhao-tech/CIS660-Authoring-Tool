//Quadrangulation Reducing Points
#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSimple.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDGModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MStringArray.h>
#include <list>
#include <iostream>
#include "half_edge.h"
#include "glm/glm.hpp"
#include "maya_ui.h"
void reducePoints(int input[],int times);
void Test();


//Plugin Initialization
MStatus initializePlugin(MObject obj)
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin(obj, "MyPlugin", "1.0", "Any");

    char buffer[2048];

    MString pluginPath = plugin.loadPath();

    MString menuPath = MString("source \"") + pluginPath + MString("/ui.mel\"");

    sprintf_s(buffer, 2048, menuPath.asChar(), pluginPath);

    MGlobal::executeCommand(buffer, true);

    // Register Command
    status = plugin.registerCommand("maya_ui_cmd", maya_ui::creator);
    if (!status) {
        status.perror("registerCommand");
        return status;
    }

    return status;
}

//Unintialize plugin
MStatus uninitializePlugin(MObject obj)
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin(obj);

    status = plugin.deregisterCommand("maya_ui_cmd");
    if (!status) {
        status.perror("deregisterCommand");
        return status;
    }

    return status;
}

//helper function
void printout(glm::vec3 val) {
    std::cout << val.x << " " << val.y << " " << val.z << std::endl;
}

//Each index in the input represents the number of edges in one side
//Now we suppose it is a triangle which means that there are at most 3 values in the array
//The length of input must be less than or equal to 6
void reducePoints(int input[], int times) {
    for (int i = 0; i < 3; i++) {
        if (input[i] == 1 || input[(i + 2) % 3] == 1) {
            continue;
        }
        int d = std::min(input[i],input[(i + 2) % 3]) - 1;
        input[i] -= d;
        input[(i + 2) % 3] -= d;
        std::cout << "formed quadrangulation: " << d << " " << input[(i+1)%3] << std::endl;
        std::cout << "rest input("<< times <<"): " << input[0] << " " << input[1] << " " << input[2] << std::endl;
    }
    int count = 0;
    for (int i = 0; i < 3; i++) {
        if (input[i] == 1) {
            count++;
        }
    }
    if (count >= 2) {
        return;
    }
    else {
        reducePoints(input,times+1);
    }
    return;
}

//create a triangle and decompose it into several pieces.
void Test(){
    //create three vertices
    Vertex* v1 = new Vertex();
    v1->pos = glm::vec3(0, 0, 0);
    Vertex* v2 = new Vertex();
    v2->pos = glm::vec3(1, 0, 0);
    Vertex* v3 = new Vertex();
    v3->pos = glm::vec3(0, 1, 0);
    std::cout << v1->pos.x << " " << v1->pos.y << " " << v1->pos.z << std::endl;

    std::vector<Vertex*> vertex_arr;
    vertex_arr.push_back(v1);
    vertex_arr.push_back(v2);
    vertex_arr.push_back(v3);

    //create edges and inverse edges
    std::vector<HalfEdge*> edge_arr;
    HalfEdge* he1 = new HalfEdge();
    he1->setVert(v1);
    HalfEdge* he2 = new HalfEdge();
    he2->setVert(v2);
    HalfEdge* he3 = new HalfEdge();
    he3->setVert(v3);

    HalfEdge* he1_inv = new HalfEdge();
    he1_inv->setVert(v3);
    HalfEdge* he2_inv = new HalfEdge();
    he2_inv->setVert(v1);
    HalfEdge* he3_inv = new HalfEdge();
    he3_inv->setVert(v2);
    he1->setSymEdge(he1_inv);
    he2->setSymEdge(he2_inv);
    he3->setSymEdge(he3_inv);

    he1->setNextEdge(he2);
    he2->setNextEdge(he3);
    he3->setNextEdge(he1);

    Face* face = new Face();
    face->setEdge(he1);

    //Quadrangulation
    //1. get the center
    glm::vec3 center_pos = face->getCenter();
    Vertex* center = new Vertex();
    center->pos = center_pos;
    
    //2. get the edges points from center to center of each edge
    
    std::vector<HalfEdge*> hf_arr = face->getHalfEdges();
    std::vector<HalfEdge*> center_edge;
    std::vector<HalfEdge*> center_edge_inv;
    std::vector<Vertex*> center_vertex;
    for (int i = 0; i < hf_arr.size(); i++) {
        HalfEdge* edge = new HalfEdge();
        HalfEdge* edge_inv = new HalfEdge();
        Vertex* v = new Vertex;
        Vertex* last = hf_arr.at(i)->getVerts().first;
        Vertex* next = hf_arr.at(i)->getVerts().second;
        glm::vec3 mid = (last->pos + next->pos) / 2.f;
        v->pos = mid;
        center_vertex.push_back(v);
        edge->setVert(v);
        edge_inv->setVert(center);
        edge->setSymEdge(edge_inv);
        center_edge.push_back(edge);
    }
    
    //test
    for (int i = 0; i < center_edge.size(); i++) {
        Vertex* first = center_edge.at(i)->getVerts().first;
        Vertex* last = center_edge.at(i)->getVerts().second;
        std::cout << "first" << first->pos.x << " " << first->pos.y << " " << first->pos.z << std::endl;
        std::cout << "last" << last->pos.x << " " << last->pos.y << " " << last->pos.z << std::endl;
    }

    //create new faces
    std::vector<Face*> face_arr;
    for (int i = 0; i < center_edge.size(); i++) {
        Face* break_face = new Face();
        HalfEdge* current = center_edge.at(i);
        HalfEdge* next = new HalfEdge(hf_arr.at(i));
        HalfEdge* next_inv = new HalfEdge();
        next_inv->setVert(current->getVerts().second);
        HalfEdge* next_next = new HalfEdge(hf_arr.at((i + 1) % hf_arr.size()));
        HalfEdge* next_next_inv = new HalfEdge();
        next_next->setVert(center_edge.at((i + 1) % hf_arr.size())->getVerts().second);
        next_next_inv->setVert(next->getVerts().second);
        break_face->setEdge(current);
        next->setFace(break_face);
        next->setNextEdge(next_next);
        next->setSymEdge(next_inv);
        current->setNextEdge(next);
        next_next->setNextEdge(center_edge.at((i + 1) % hf_arr.size())->getSymHalfEdge());
        center_edge.at((i + 1) % hf_arr.size())->getSymHalfEdge()->setNextEdge(current);
        face_arr.push_back(break_face);
        /*std::cout << "current edges" << std::endl;
        printout(next->getVerts().first->pos);
        printout(next->getVerts().second->pos);*/
    }
    
    std::cout << "size " << face_arr.size() << std::endl;
    Face* temp = face_arr.at(1);
    std::vector<HalfEdge*> half_arr = temp->getHalfEdges();
    std::cout << "half edge size " << half_arr.size() << std::endl;
    for (int i = 0; i < half_arr.size(); i++) {
        Vertex* no1 = half_arr.at(i)->getVerts().first;
        Vertex* no2 = half_arr.at(i)->getVerts().second;
        std::cout << "edges" << std::endl;
        std::cout << "no1 " << no1->pos.x << " " << no1->pos.y << " " << no1->pos.z << std::endl;
        std::cout << "no2 " << no2->pos.x << " " << no2->pos.y << " " << no2->pos.z << std::endl;
    }
}


