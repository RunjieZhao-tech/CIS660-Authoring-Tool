#include <fstream>
#include <sstream>
#include <vector>
#include "maya_node.h"
#include <maya/MGlobal.h>

MObject MayaNode::inputPositions;
MObject MayaNode::outputGeometry;
MObject MayaNode::radius;
MObject MayaNode::degree;
MObject MayaNode::step;
MObject MayaNode::time;
MObject MayaNode::vertex_num;
MTypeId MayaNode::id(0x80000);
MObject MayaNode::inputGeometry;

void* MayaNode::creator() {
	return new MayaNode();
}

MStatus MayaNode::compute(const MPlug& plug, MDataBlock& data) {
	MStatus returnStatus;
	if (plug == outputGeometry) {
		//retrieve the data
		MDataHandle positionHandle = data.inputValue(inputPositions, &returnStatus);
		McheckErr(returnStatus, "Error getting positions data handle\n");
		MString positionString = positionHandle.asString();
		if (positionString == "") {
			return MS::kSuccess;
		}
		
		//degree
		MDataHandle degreeHandle = data.inputValue(degree, &returnStatus);
		McheckErr(returnStatus, "Error getting degree data handle\n");
		double DEG = degreeHandle.asDouble();
		

		//number of vertices
		MDataHandle vertexHandle = data.outputValue(vertex_num,&returnStatus);
		McheckErr(returnStatus, "Error getting number of vertices data handle\n");
		double Vertices = vertexHandle.asDouble();
		std::string info = "in the maya node the numV value is " + std::to_string(Vertices);
		MGlobal::displayInfo(info.c_str());
		//step
		MDataHandle stepHandle = data.inputValue(step, &returnStatus);
		McheckErr(returnStatus, "Error getting angle data handle\n");
		double STEP = stepHandle.asDouble();

		//time
		MDataHandle timeHandle = data.inputValue(time, &returnStatus);
		McheckErr(returnStatus, "Error getting angle data handle\n");
		MTime TIME = timeHandle.asTime();
		int iteration = TIME.value();

		//input geometry
		MDataHandle fileHandle = data.inputValue(inputGeometry, &returnStatus);
		MString inputFile = fileHandle.asString();
		McheckErr(returnStatus, "ERROR: fileHandle\n");
		if (inputFile == "") { 
			MGlobal::displayInfo("No info!");
			return MS::kSuccess; 
		}
		else {
			MGlobal::displayInfo(inputFile);
			return MS::kSuccess;
		}

		//output geometry
		MDataHandle outputGeometryHandle = data.outputValue(outputGeometry, &returnStatus);
		McheckErr(returnStatus, "Error getting geometry data handle\n");

		//output data
		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "Error Getting Output Data");

		//outputGeometryHandle.set(newOutputData);
		data.setClean(plug);

	}
	MGlobal::displayInfo("compute!");
	return returnStatus;
}

MStatus MayaNode::initialize() {
	MFnTypedAttribute geomAttr;
	MStatus ms;
	MFnNumericAttribute sizeAttr;
	MFnUnitAttribute unitAttr;
	MStatus returnStatus;

	//set attributes
	MayaNode::inputPositions = geomAttr.create("input_positions", "pos", MFnData::kString, MObject::kNullObj, &returnStatus);
	McheckErr(returnStatus, "Error creating node input position attribute\n");

	MayaNode::vertex_num = sizeAttr.create("NumV","nv", MFnNumericData::kDouble,0,&returnStatus);
	McheckErr(returnStatus, "Error creating number of nodes attribute\n");

	MayaNode::degree = sizeAttr.create("degree", "deg", MFnNumericData::kDouble, 0, &returnStatus);
	McheckErr(returnStatus, "Error creating lsystem node degree attribute\n");

	MayaNode::step = sizeAttr.create("step", "s", MFnNumericData::kDouble, 0, &returnStatus);
	McheckErr(returnStatus, "Error creating lsystem node angle attribute\n");

	MayaNode::time = unitAttr.create("time", "t", MFnUnitAttribute::kTime, 0, &returnStatus);
	McheckErr(returnStatus, "Error creating lsystem node angle attribute\n");

	MayaNode::outputGeometry = geomAttr.create("output_geometry", "o_geom", MFnData::kMesh, MObject::kNullObj, &returnStatus);
	McheckErr(returnStatus, "Error creating lsystem output geometry attribute\n");

	MayaNode::inputGeometry = geomAttr.create("input_geometry", "i_geom", MFnData::kString);


	//add attributes
	returnStatus = addAttribute(MayaNode::inputPositions);
	McheckErr(returnStatus, "Error adding input geometry attribute");

	returnStatus = addAttribute(MayaNode::vertex_num);
	McheckErr(returnStatus, "Error adding number of vertex geometry attribute");
	//returnStatus = addAttribute(LSystemNode::radius);
	//McheckErr(returnStatus,"Error adding size attribute");

	returnStatus = addAttribute(MayaNode::degree);
	McheckErr(returnStatus, "Error adding degree attribute");

	returnStatus = addAttribute(MayaNode::step);
	McheckErr(returnStatus, "Error adding angle attribute");

	returnStatus = addAttribute(MayaNode::time);
	McheckErr(returnStatus, "Error adding time attribute");

	returnStatus = addAttribute(MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding output geometry attribute");

	returnStatus = addAttribute(MayaNode::inputGeometry);
	McheckErr(returnStatus, "Error adding input geometry attribute");
	//set affect attributes
	returnStatus = attributeAffects(MayaNode::inputPositions, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding input position attributeAffect");

	returnStatus = attributeAffects(MayaNode::degree, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding degree attributeAffect");

	returnStatus = attributeAffects(MayaNode::vertex_num, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding vertex number attributeAffect");

	returnStatus = attributeAffects(MayaNode::step, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding angle attributeAffect");

	returnStatus = attributeAffects(MayaNode::time, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding time attributeAffect");

	returnStatus = attributeAffects(MayaNode::degree, MayaNode::step);
	McheckErr(returnStatus, "Error adding time attributeAffect");

	return MS::kSuccess;
}