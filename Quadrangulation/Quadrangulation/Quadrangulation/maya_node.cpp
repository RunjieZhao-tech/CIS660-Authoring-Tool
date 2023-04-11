#include <fstream>
#include <sstream>
#include <vector>
#include "maya_node.h"
#include <maya/MGlobal.h>

//
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnComponentListData.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MFnEnumAttribute.h>

// General Includes
//
#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MIOStream.h>

//MObject MayaNode::inputPositions;
//
//MObject MayaNode::radius;
//MObject MayaNode::time;
//MObject MayaNode::vertex_num;
MTypeId MayaNode::id(0x80000);


MObject MayaNode::maxOccurency1;
MObject MayaNode::maxOccurency2;
MObject MayaNode::maxOccurency3;
MObject MayaNode::maxOccurency4;
MObject MayaNode::weight1;
MObject MayaNode::weight2;
MObject MayaNode::weight3;
MObject MayaNode::weight4;
MObject MayaNode::inputGeometry;
MObject MayaNode::outputGeometry;


void* MayaNode::creator() {
	return new MayaNode();
}

MStatus MayaNode::compute(const MPlug& plug, MDataBlock& data) {
	MStatus returnStatus;
	
		//retrieve the data
		//MDataHandle positionHandle = data.inputValue(inputPositions, &returnStatus);
		//McheckErr(returnStatus, "Error getting positions data handle\n");
		//MString positionString = positionHandle.asString();
		//if (positionString == "") {
		//	return MS::kSuccess;
		//}
		//
		//MGlobal::displayInfo(positionString);

		//degree
		//MDataHandle degreeHandle = data.inputValue(maxOccurency1, &returnStatus);
		//McheckErr(returnStatus, "Error getting degree data handle\n");
		//double DEG = degreeHandle.asDouble();
		
		//number of vertices
		//MDataHandle vertexHandle = data.outputValue(vertex_num,&returnStatus);
		//McheckErr(returnStatus, "Error getting number of vertices data handle\n");
		//double Vertices = vertexHandle.asDouble();
		//std::string info = "in the maya node the numV value is " + std::to_string(Vertices);
		//MGlobal::displayInfo(info.c_str());
		//step
		//MDataHandle stepHandle = data.inputValue(weight1, &returnStatus);
		//McheckErr(returnStatus, "Error getting angle data handle\n");
		//double STEP = stepHandle.asDouble();

		//time
		//MDataHandle timeHandle = data.inputValue(time, &returnStatus);
		//McheckErr(returnStatus, "Error getting angle data handle\n");
		//MTime TIME = timeHandle.asTime();
		//int iteration = TIME.value();

		//input geometry
		//MDataHandle fileHandle = data.inputValue(inputGeometry, &returnStatus);
		//MString inputFile = fileHandle.asString();
		//MObject mesh = fileHandle.asMesh();

		//McheckErr(returnStatus, "ERROR: fileHandle\n");
		//if (inputFile == "") { 
		//	MGlobal::displayInfo("No info!");
		//	return MS::kSuccess; 
		//}
		//else {
		//	MGlobal::displayInfo(inputFile);
		//	return MS::kSuccess;
		//}

		////output geometry
		//MDataHandle outputGeometryHandle = data.outputValue(outputGeometry, &returnStatus);
		//McheckErr(returnStatus, "Error getting geometry data handle\n");

		////output data
		//MFnMeshData dataCreator;
		//MObject newOutputData = dataCreator.create(&returnStatus);
		//McheckErr(returnStatus, "Error Getting Output Data");

		//outputGeometryHandle.set(newOutputData);

		//
	MDataHandle oponentList = data.inputValue(inputGeometry, &returnStatus);
	McheckErr(returnStatus, "Error getting geometry data handle\n");
	//MObject mesh = oponentList.asMesh();
	MFnMesh mesh(oponentList.asMesh());
	MGlobal::displayInfo("numVertices: " + mesh.numVertices());
	MGlobal::displayInfo("numEdges: " + mesh.numEdges());
	MGlobal::displayInfo("numPolygons: " + mesh.numPolygons());
	MGlobal::displayInfo("numFaceVertices: " + mesh.numFaceVertices());
	MFloatPointArray pts = MFloatPointArray();
	MIntArray vertexCount = MIntArray();
	MIntArray vertexList = MIntArray();
	mesh.getPoints(pts);
	int count = 0;
	for (auto pt : pts) {
		count++;
		std::string str = "getPoints: " + std::to_string(pt.x) + ", " + std::to_string(pt.y) + ", " + std::to_string(pt.z) + ", " + std::to_string(pt.w);
		MGlobal::displayInfo(MString(str.c_str()));
	}
	std::string countStr = std::to_string(count);
	MGlobal::displayInfo(countStr.c_str());
	count = 0;

	mesh.getVertices(vertexCount, vertexList);
	MGlobal::displayInfo(MString("getVertices"));
	for (auto vc : vertexCount) {
		count++;
		std::string str = "vertexCount: " + std::to_string(vc);
		MGlobal::displayInfo(MString(str.c_str()));
	}
	countStr = std::to_string(count);
	MGlobal::displayInfo(countStr.c_str());
	count = 0;

	for (auto vc : vertexList) {
		count++;
		std::string str = "vertexList: " + std::to_string(vc);
		MGlobal::displayInfo(MString(str.c_str()));
	}
	countStr = std::to_string(count);
	MGlobal::displayInfo(countStr.c_str());
	count = 0;

	MGlobal::displayInfo(MString("getPolygonVertices"));
	for (int i = 0;i < mesh.numPolygons();i++) {
		MIntArray vertexListf = MIntArray();
		mesh.getPolygonVertices(i, vertexListf);
		std::string str = "Face " + std::to_string(i) + ": ";
		for (auto v : vertexListf) {
			str += (std::to_string(v) + " ");
		}
		MGlobal::displayInfo(MString(str.c_str()));
	}


	//https://download.autodesk.com/us/maya/2011help/API/class_m_fn_mesh.html
		//int 	numVertices(MStatus * ReturnStatus = NULL) const
		//int 	numEdges(MStatus * ReturnStatus = NULL) const
		//int 	numPolygons(MStatus * ReturnStatus = NULL) const
		//int 	numFaceVertices(MStatus * ReturnStatus = NULL) const
		//int 	polygonVertexCount(int polygonId, MStatus * ReturnStatus = NULL) const
	//MStatus 	getPoints(MFloatPointArray & vertexArray, MSpace::Space space = MSpace::kObject) const
	//	MStatus 	getPoints(MPointArray & vertexArray, MSpace::Space space = MSpace::kObject) const
	//	MStatus 	getVertices(MIntArray & vertexCount, MIntArray & vertexList) const
	//	MStatus 	getPolygonVertices(int polygonId, MIntArray & vertexList) const
	data.setClean(plug);

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
	MayaNode::maxOccurency1 = sizeAttr.create("max_occurency_1", "max_occur_1", MFnNumericData::kInt, -1, &returnStatus);
	McheckErr(returnStatus, "Error creating node max_occurency_1 attribute\n");
	MayaNode::maxOccurency2 = sizeAttr.create("max_occurency_2", "max_occur_2", MFnNumericData::kInt, -1, &returnStatus);
	McheckErr(returnStatus, "Error creating node max_occurency_2 attribute\n");
	MayaNode::maxOccurency3 = sizeAttr.create("max_occurency_3", "max_occur_3", MFnNumericData::kInt, -1, &returnStatus);
	McheckErr(returnStatus, "Error creating node max_occurency_3 attribute\n");
	MayaNode::maxOccurency4 = sizeAttr.create("max_occurency_4", "max_occur_4", MFnNumericData::kInt, -1, &returnStatus);
	McheckErr(returnStatus, "Error creating node max_occurency_4 attribute\n");

	MayaNode::weight1 = sizeAttr.create("weight_1", "w_1", MFnNumericData::kDouble, 0, &returnStatus);
	McheckErr(returnStatus, "Error creating node weight_1 attribute\n");
	MayaNode::weight2 = sizeAttr.create("weight_2", "w_2", MFnNumericData::kDouble, 0, &returnStatus);
	McheckErr(returnStatus, "Error creating node weight_2 attribute\n");
	MayaNode::weight3 = sizeAttr.create("weight_3", "w_3", MFnNumericData::kDouble, 0, &returnStatus);
	McheckErr(returnStatus, "Error creating node weight_3 attribute\n");
	MayaNode::weight4 = sizeAttr.create("weight_4", "w_4", MFnNumericData::kDouble, 0, &returnStatus);
	McheckErr(returnStatus, "Error creating node weight_4 attribute\n");

	MayaNode::inputGeometry = geomAttr.create("input_geometry", "i_geom", MFnData::kMesh, MObject::kNullObj, &returnStatus);
	McheckErr(returnStatus, "Error creating input_geometry attribute\n");
	MayaNode::outputGeometry = geomAttr.create("output_geometry", "o_geom", MFnData::kMesh, MObject::kNullObj, &returnStatus);
	McheckErr(returnStatus, "Error creating output_geometry attribute\n");

	//add attributes
	returnStatus = addAttribute(MayaNode::maxOccurency1);
	McheckErr(returnStatus, "Error adding max_occurency_1 attribute");
	returnStatus = addAttribute(MayaNode::maxOccurency2);
	McheckErr(returnStatus, "Error adding max_occurency_2 attribute");
	returnStatus = addAttribute(MayaNode::maxOccurency3);
	McheckErr(returnStatus, "Error adding max_occurency_3 attribute");
	returnStatus = addAttribute(MayaNode::maxOccurency4);
	McheckErr(returnStatus, "Error adding max_occurency_4 attribute");

	returnStatus = addAttribute(MayaNode::weight1);
	McheckErr(returnStatus, "Error adding weight_1 attribute");
	returnStatus = addAttribute(MayaNode::weight2);
	McheckErr(returnStatus, "Error adding weight_2 attribute");
	returnStatus = addAttribute(MayaNode::weight3);
	McheckErr(returnStatus, "Error adding weight_3 attribute");
	returnStatus = addAttribute(MayaNode::weight4);
	McheckErr(returnStatus, "Error adding weight_4 attribute");

	returnStatus = addAttribute(MayaNode::inputGeometry);
	McheckErr(returnStatus, "Error adding input geometry attribute");
	returnStatus = addAttribute(MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding output geometry attribute");

	//attributes affect
	returnStatus = attributeAffects(MayaNode::maxOccurency1, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding max_occurency_1 attributeAffect");
	returnStatus = attributeAffects(MayaNode::maxOccurency2, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding max_occurency_2 attributeAffect");
	returnStatus = attributeAffects(MayaNode::maxOccurency3, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding max_occurency_3 attributeAffect");
	returnStatus = attributeAffects(MayaNode::maxOccurency4, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding max_occurency_4 attributeAffect");

	returnStatus = attributeAffects(MayaNode::weight1, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding weight_1 attributeAffect");
	returnStatus = attributeAffects(MayaNode::weight2, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding weight_2 attributeAffect");
	returnStatus = attributeAffects(MayaNode::weight3, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding weight_3 attributeAffect");
	returnStatus = attributeAffects(MayaNode::weight4, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding weight_4 attributeAffect");

	//test only
	returnStatus = attributeAffects(MayaNode::weight4, MayaNode::weight3);

	//MayaNode::inputPositions = geomAttr.create("input_positions", "pos", MFnData::kString, MObject::kNullObj, &returnStatus);
	//McheckErr(returnStatus, "Error creating node input position attribute\n");

	//MayaNode::vertex_num = sizeAttr.create("NumV","nv", MFnNumericData::kDouble,0,&returnStatus);
	//McheckErr(returnStatus, "Error creating number of nodes attribute\n");

	//MayaNode::time = unitAttr.create("time", "t", MFnUnitAttribute::kTime, 0, &returnStatus);
	//McheckErr(returnStatus, "Error creating lsystem node angle attribute\n");
	//MayaNode::inputGeometry = geomAttr.create("input_geometry", "i_geom", MFnData::kString);


	//add attributes
	//returnStatus = addAttribute(MayaNode::inputPositions);
	//McheckErr(returnStatus, "Error adding input geometry attribute");

	//returnStatus = addAttribute(MayaNode::vertex_num);
	//McheckErr(returnStatus, "Error adding number of vertex geometry attribute");
	//returnStatus = addAttribute(LSystemNode::radius);
	//McheckErr(returnStatus,"Error adding size attribute");

	//returnStatus = addAttribute(MayaNode::time);
	//McheckErr(returnStatus, "Error adding time attribute");

	//returnStatus = addAttribute(MayaNode::outputGeometry);
	//McheckErr(returnStatus, "Error adding output geometry attribute");

	//returnStatus = addAttribute(MayaNode::inputGeometry);
	//McheckErr(returnStatus, "Error adding input geometry attribute");
	////set affect attributes
	//returnStatus = attributeAffects(MayaNode::inputPositions, MayaNode::outputGeometry);
	//McheckErr(returnStatus, "Error adding input position attributeAffect");

	//returnStatus = attributeAffects(MayaNode::vertex_num, MayaNode::outputGeometry);
	//McheckErr(returnStatus, "Error adding vertex number attributeAffect");

	//returnStatus = attributeAffects(MayaNode::weight1, MayaNode::outputGeometry);
	//McheckErr(returnStatus, "Error adding angle attributeAffect");

	//returnStatus = attributeAffects(MayaNode::time, MayaNode::outputGeometry);
	//McheckErr(returnStatus, "Error adding time attributeAffect");

	return MS::kSuccess;
}