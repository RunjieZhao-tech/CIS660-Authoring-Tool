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
//#include "half_edge.h"
//#include "glm/glm.hpp"
#include "maya_ui.h"
#include "maya_node.h"
//#include "tile_solver.h"
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
    //Register Node
    status = plugin.registerNode("MayaNode", MayaNode::id, MayaNode::creator, MayaNode::initialize);
    if (!status) {
        status.perror("registerNode");
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

    status = plugin.deregisterNode(MayaNode::id);
    if (!status) {
        status.perror("deregisterNode");
        return status;
    }

    return status;
}

//helper function
/*void printout(glm::vec3 val) {
    std::cout << val.x << " " << val.y << " " << val.z << std::endl;
}*/

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


