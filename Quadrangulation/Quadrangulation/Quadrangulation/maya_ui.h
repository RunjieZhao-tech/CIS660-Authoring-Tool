#pragma once
#include <maya/MArgList.h> 
#include <maya/MObject.h> 
#include <maya/MGlobal.h> 
#include <maya/MPxCommand.h> 
#include <string>

class maya_ui : public MPxCommand
{
public:
    maya_ui();
    virtual ~maya_ui();
    static void* creator() { return new maya_ui(); }
    MStatus doIt(const MArgList& args);
};