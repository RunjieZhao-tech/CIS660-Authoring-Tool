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
#include <maya/MPointArray.h>

//MObject MayaNode::inputPositions;
//
//MObject MayaNode::radius;
//MObject MayaNode::time;
//MObject MayaNode::vertex_num;
bool attri = true;

MTypeId MayaNode::id(0x80000);

MObject MayaNode::inputFile;
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
//Helper Function
std::vector<std::vector<bool>> initialize_vector() {
	std::vector<bool> row(5,false);
	std::vector<std::vector<bool>> res(5,row);
	return res;
}

//has tiled function
bool hasTiled(const Face* QuadWantTile, const std::vector<Face*>& tiledFace) {
	for (auto tile : tiledFace) {
		if (tile == QuadWantTile)return true;
	}
	return false;
}

//given a box with customized tiles and all faces, find how the possibilities of how it can be fit into the template
std::vector<std::vector<Face*>> possibleTiles(std::vector<Face*>& quads, std::vector<std::vector<bool>> box) {
	std::vector<std::vector<Face*>> result;
	for (auto quad : quads) {
		std::vector<HalfEdge*> halfEdges = quad->getHalfEdges();
		for (auto halfEdge : halfEdges) {
			std::vector<Face*> possibleTile;
			//test new algorithm
			bool empty = false;
			//iterate the customized tile in the box;
			for (int i = 0; i < box.size(); i++) {

				for (int j = 0; j < box.size(); j++) {
					HalfEdge* bot = halfEdge->getNextHalfEdge()->getNextHalfEdge()->getNextHalfEdge();
					if (box[i][j] == false) {
						continue;
					}
					for (int r = 0; r < i; r++) {
						bot = bot->getSymHalfEdge();
						bot = bot->getNextHalfEdge()->getNextHalfEdge();
						if (bot->getFace() == nullptr) {
							empty = true;
							break;
						}
					}
					if (empty) {
						break;
					}
					for (int c = 0; c < j; c++) {
						bot = bot->getNextHalfEdge()->getSymHalfEdge();
						if (bot->getFace() == nullptr) {
							empty = true;
							break;
						}
						bot = bot->getNextHalfEdge();
					}
					if (empty) {
						break;
					}
					Face* possibleface = bot->getFace();
					if (hasTiled(possibleface, possibleTile)) {
						empty = true;
						break;
					}
					else {
						possibleTile.push_back(possibleface);
					}
				}
				if (empty) {
					break;
				}
			}
			if (empty)continue;
			result.push_back(possibleTile);
		}

	}
	return result;
}

void* MayaNode::creator() {
	return new MayaNode();
}

MStatus MayaNode::compute(const MPlug& plug, MDataBlock& data) {
	MStatus returnStatus;
	if (plug == outputGeometry) {
		MDataHandle oponentList = data.inputValue(inputGeometry, &returnStatus);
		McheckErr(returnStatus, "Error getting geometry data handle\n");
			//handle the input file location
	MDataHandle fileHandle = data.inputValue(inputFile,&returnStatus);
	McheckErr(returnStatus, "Error getting file position handle\n");
	MString filelocation = fileHandle.asString();
	MGlobal::displayInfo("file location: " + filelocation);
	std::ifstream myfile(filelocation.asChar());
	
	//initialize all the tiles
	std::vector<std::vector<std::vector<bool>>> all_tiles;
	
	//allow the program to read files
	if (myfile.is_open()) {
		std::string mystring;
		//read each row in each while loop
		while (myfile.good()) {
			myfile >> mystring;
			std::vector<std::vector<bool>> tile = initialize_vector();
			//iterate through each row to get the quads face
			int row_num = 0;
			int col_num = 0;
			for (int i = 0; i < mystring.length(); i++) {
				if (mystring[i] == '/') {
					row_num++;
					col_num = 0;
					continue;
				}
				if (mystring[i] == 'F') {
					tile[row_num][col_num] = true;
					col_num++;
				}
			}
			all_tiles.push_back(tile);
			MGlobal::displayInfo(mystring.c_str());
		}

		//iterate through all tiles
		//used for testing
		for (int i = 0; i < all_tiles.size(); i++) {
			for (int j = 0; j < all_tiles.at(i).size(); j++) {
				for (int k = 0; k < all_tiles.at(i).at(j).size(); k++) {
					if (all_tiles[i][j][k]) {
						MGlobal::displayInfo("True");
					}
					else {
						MGlobal::displayInfo("False");
					}
				}
			}
		}
	}
	myfile.close();
	
	//add weight based on the input
	/*if (attri) {
		MFnNumericAttribute sizeAttr;
		MObject testing =  sizeAttr.create("test", "test_1", MFnNumericData::kInt, -1, &returnStatus);
		McheckErr(returnStatus, "Error creating testing attribte\n");
		returnStatus = addAttribute(testing);
		McheckErr(returnStatus, "Error adding testing attribute");
		returnStatus = attributeAffects(testing, MayaNode::outputGeometry);
		McheckErr(returnStatus, "Error testing attributeAffect");
		MGlobal::displayInfo("Im in the if statement where i will add a new attribute\n");
		attri = false;
	}*/

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

		//max occurency
		MDataHandle
		maxOccurHandle = data.inputValue(maxOccurency1, &returnStatus);
		McheckErr(returnStatus, "Error getting maxOccurency1 data handle\n");
		int maxOccur1 = maxOccurHandle.asInt();
		maxOccurHandle = data.inputValue(maxOccurency2, &returnStatus);
		McheckErr(returnStatus, "Error getting maxOccurency2 data handle\n");
		int maxOccur2 = maxOccurHandle.asInt();
		maxOccurHandle = data.inputValue(maxOccurency3, &returnStatus);
		McheckErr(returnStatus, "Error getting maxOccurency3 data handle\n");
		int maxOccur3 = maxOccurHandle.asInt();
		maxOccurHandle = data.inputValue(maxOccurency4, &returnStatus);
		McheckErr(returnStatus, "Error getting maxOccurency4 data handle\n");
		int maxOccur4 = maxOccurHandle.asInt();

		//weights
		MDataHandle
		weightHandle = data.inputValue(weight1, &returnStatus);
		McheckErr(returnStatus, "Error getting weight1 data handle\n");
		float w1 = weightHandle.asFloat();

		weightHandle = data.inputValue(weight2, &returnStatus);
		McheckErr(returnStatus, "Error getting weight2 data handle\n");
		float w2 = weightHandle.asFloat();

		MDataHandle weightHandle3 = data.inputValue(weight3, &returnStatus);
		McheckErr(returnStatus, "Error getting weight3 data handle\n");
		float w3 = weightHandle3.asFloat();

		weightHandle = data.inputValue(weight4, &returnStatus);
		McheckErr(returnStatus, "Error getting weight4 data handle\n");
		float w4 = weightHandle.asFloat();

		//construct patch
		MIntArray vertexListf = MIntArray();
		mesh.getPolygonVertices(0, vertexListf);
		std::vector<glm::vec3> verts;
		for (auto id : vertexListf) {
			verts.push_back(glm::vec3(pts[id].x, pts[id].y, pts[id].z));
		}
		Patch patch(verts);
		patch.quadrangulate();
		MFnMeshData meshData;
		MObject newOutputGeom = meshData.create(&returnStatus);
		McheckErr(returnStatus, "Error creating geometry data\n");
		weightHandle = data.outputValue(outputGeometry, &returnStatus);
		McheckErr(returnStatus, "Error getting geometry data handle\n");

		std::vector<Tile*> tiles;
		for (int i = 0; i < all_tiles.size(); i++) {
			LTile* t = new LTile();
			t->box = all_tiles.at(i);
			tiles.push_back(t);
		}
		I2Tile tile1;
		I3Tile tile2;
		LTile tile3;
		SquareTile tile4;

		/*if (maxOccur1 != 0) {
			tile1.maxOccurence = maxOccur1;
			tile1.weight = w1;
			tiles.push_back(&tile1);
		}
		if (maxOccur2 != 0) {
			tile2.maxOccurence = maxOccur2;
			tile2.weight = w2;
			tiles.push_back(&tile2);
		}
		if (maxOccur3 != 0) {
			tile3.maxOccurence = maxOccur3;
			tile3.weight = w3;
			tiles.push_back(&tile3);
		}
		if (maxOccur4 != 0) {
			tile4.maxOccurence = maxOccur4;
			tile4.weight = w4;
			tiles.push_back(&tile4);
		}*/

		TileSolver solver;
		std::vector<Face*> quad;
		for (auto&& q : patch.quads) {
			quad.push_back(q.get());
		}

		std::vector<std::vector<Face*>> result = solver.solveTiling(quad, tiles);
		std::unordered_map<Vertex*, int> vertIndexMap;
		MPointArray mPos = MPointArray();
		MIntArray faceCounts = MIntArray();
		MIntArray faceConnects = MIntArray();
		MVectorArray mNormal = MVectorArray();
		for (auto tile : result) {
			auto verts = buildPolygon(tile);
			faceCounts.append(verts.size());//how many edges in the polygon
			for (auto vert : verts) {
				bool notInArray = (vertIndexMap.find(vert) == vertIndexMap.end());
				if (notInArray) {
					vertIndexMap[vert] = mPos.length();
					//since the normal and postion is the same
					mPos.append(MPoint(vert->pos.x, vert->pos.y, vert->pos.z));
					mNormal.append(MVector(0, 1, 0));
				}
				faceConnects.append(vertIndexMap[vert]);
			}
		}

		//mesh manipulation
		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR: creating output data\n");
		MFnMesh meshFS;
		meshFS.create(mPos.length(), faceCounts.length(), mPos, faceCounts, faceConnects, newOutputData, &returnStatus);
		McheckErr(returnStatus, "ERROR: creating new geometry");

		//output handle
		MDataHandle outGeomHandle = data.outputValue(outputGeometry, &returnStatus);
		McheckErr(returnStatus, "ERROR: geometry data handle\n");
		outGeomHandle.set(newOutputData);
		////https://download.autodesk.com/us/maya/2011help/API/class_m_fn_mesh.html
		//	//int 	numVertices(MStatus * ReturnStatus = NULL) const
		//	//int 	numEdges(MStatus * ReturnStatus = NULL) const
		//	//int 	numPolygons(MStatus * ReturnStatus = NULL) const
		//	//int 	numFaceVertices(MStatus * ReturnStatus = NULL) const
		//	//int 	polygonVertexCount(int polygonId, MStatus * ReturnStatus = NULL) const
		////MStatus 	getPoints(MFloatPointArray & vertexArray, MSpace::Space space = MSpace::kObject) const
		////	MStatus 	getPoints(MPointArray & vertexArray, MSpace::Space space = MSpace::kObject) const
		////	MStatus 	getVertices(MIntArray & vertexCount, MIntArray & vertexList) const
		////	MStatus 	getPolygonVertices(int polygonId, MIntArray & vertexList) const
		data.setClean(plug);
	}
	else {
		return MS::kUnknownParameter;
	}
	MGlobal::displayInfo("compute!");
	return returnStatus;
}

MStatus MayaNode::initialize() {
	MFnTypedAttribute geomAttr;
	MStatus ms;
	MFnNumericAttribute sizeAttr;
	MFnNumericAttribute weightAttr;
	MFnUnitAttribute unitAttr;
	MStatus returnStatus;

	//set attributes
	//set input file location
	MayaNode::inputFile = geomAttr.create("InputFile", "inputfile", MFnData::kString,MObject::kNullObj,&returnStatus);
	McheckErr(returnStatus,"Error creating input file attribte\n");	
	//set occurrencies
	MayaNode::maxOccurency1 = sizeAttr.create("max_occurency_1", "max_occur_1", MFnNumericData::kInt, -1, &returnStatus);
	McheckErr(returnStatus, "Error creating node max_occurency_1 attribute\n");
	MayaNode::maxOccurency2 = sizeAttr.create("max_occurency_2", "max_occur_2", MFnNumericData::kInt, -1, &returnStatus);
	McheckErr(returnStatus, "Error creating node max_occurency_2 attribute\n");
	MayaNode::maxOccurency3 = sizeAttr.create("max_occurency_3", "max_occur_3", MFnNumericData::kInt, -1, &returnStatus);
	McheckErr(returnStatus, "Error creating node max_occurency_3 attribute\n");
	MayaNode::maxOccurency4 = sizeAttr.create("max_occurency_4", "max_occur_4", MFnNumericData::kInt, -1, &returnStatus);
	McheckErr(returnStatus, "Error creating node max_occurency_4 attribute\n");

	MayaNode::weight1 = weightAttr.create("weight_1", "w_1", MFnNumericData::kFloat, 0.f, &returnStatus);
	McheckErr(returnStatus, "Error creating node weight_1 attribute\n");
	MayaNode::weight2 = weightAttr.create("weight_2", "w_2", MFnNumericData::kFloat, 0.f, &returnStatus);
	McheckErr(returnStatus, "Error creating node weight_2 attribute\n");
	MayaNode::weight3 = weightAttr.create("weight_3", "w_3", MFnNumericData::kFloat, 0.f, &returnStatus);
	McheckErr(returnStatus, "Error creating node weight_3 attribute\n");
	MayaNode::weight4 = weightAttr.create("weight_4", "w_4", MFnNumericData::kFloat, 0.f, &returnStatus);
	McheckErr(returnStatus, "Error creating node weight_4 attribute\n");

	MayaNode::inputGeometry = geomAttr.create("input_geometry", "i_geom", MFnData::kMesh, MObject::kNullObj, &returnStatus);
	McheckErr(returnStatus, "Error creating input_geometry attribute\n");
	MayaNode::outputGeometry = geomAttr.create("output_geometry", "o_geom", MFnData::kMesh, MObject::kNullObj, &returnStatus);
	McheckErr(returnStatus, "Error creating output_geometry attribute\n");

	//add attributes
	//add input file location attribute
	returnStatus = addAttribute(MayaNode::inputFile);
	McheckErr(returnStatus,"Error adding input files attribute");
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
	//input file position
	returnStatus = attributeAffects(MayaNode::inputFile, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding input position attributeAffect");
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

	return MS::kSuccess;
}

std::vector<Vertex*> MayaNode::buildPolygon(std::vector<Face*>& tile) {
	std::unordered_set<Face*> tileSet;
	for (auto faceInTile : tile) {
		tileSet.insert(faceInTile);
	}

	//find first bound
	HalfEdge* currEdge = tile[0]->getHalfEdges()[0];
	currEdge = currEdge->getSymHalfEdge();
	do {
		Face* notInTile = currEdge->getFace();
		if (notInTile == nullptr || tileSet.find(notInTile) == tileSet.end()) {
			break;
		}
		currEdge = currEdge->getNextHalfEdge()->getNextHalfEdge()->getSymHalfEdge();
	} while (currEdge->getFace() != tile[0]);
	HalfEdge* firstBound = currEdge->getSymHalfEdge();

	//find rest bound
	std::vector<Vertex*>result;
	currEdge = firstBound;
	do {
		result.push_back(currEdge->vert);
		currEdge = findNextBoundary(currEdge, tileSet);
	} while (currEdge != firstBound);

	return result;
}

HalfEdge* MayaNode::findNextBoundary(HalfEdge* currentEdge,const std::unordered_set<Face*>&tile) {
	HalfEdge* candidate = currentEdge->getNextHalfEdge();
	for(int i=0;i<4;i++){
		Face* notInTile = candidate->getSymHalfEdge()->getFace();
		if (notInTile == nullptr || tile.find(notInTile) == tile.end()) {
			return candidate;
		}
		candidate = candidate->getSymHalfEdge()->getNextHalfEdge();
	}
	return candidate;
}