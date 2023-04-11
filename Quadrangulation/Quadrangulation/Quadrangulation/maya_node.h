#pragma once

#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnMesh.h>

#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MFnMeshData.h>

#include <maya/MIOStream.h>

#define McheckErr(stat,msg) \
	if (MS::kSuccess != stat) { \
		cerr << msg;		   \
		return MS::kFailure; \
	} \

class MayaNode : public MPxNode {
public:
	MayaNode() {};
	~MayaNode() override {};
	MStatus compute(const MPlug& plug, MDataBlock& data) override;
	static void* creator();
	static MStatus initialize();

	//static MObject inputPositions;
	
	//static MObject radius;
	//static MObject time;
	//static MObject vertex_num;
	static MTypeId id;
	
	static MObject maxOccurency1;
	static MObject maxOccurency2;
	static MObject maxOccurency3;
	static MObject maxOccurency4;

	static MObject weight1;
	static MObject weight2;
	static MObject weight3;
	static MObject weight4;

	static MObject inputGeometry;
	static MObject outputGeometry;

};