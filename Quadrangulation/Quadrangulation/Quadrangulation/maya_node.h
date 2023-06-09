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
#include "half_edge.h"
#include "tile_solver.h"
#include <unordered_set>

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
	static std::vector<MObject> occur;
	static MTypeId id;
	
	static MObject myListAttr;
	static MObject inputFile;
	static MObject weight;
	static MObject maxOccurency;
	
	static MObject tileSideLen;
	
	static MObject inputGeometry;
	static MObject outputGeometry;
	static MObject tile_display;

	static MObject dropList;
	static std::vector<std::string> allTile;

	static std::vector<Vertex*> buildPolygon(std::vector<Face*>& Tile);
	static HalfEdge* findNextBoundary(HalfEdge* currentEdge, const std::unordered_set<Face*>& tile);
};