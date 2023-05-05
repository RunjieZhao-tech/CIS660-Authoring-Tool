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
#include <maya/MFnCompoundAttribute.h>

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


MObject MayaNode::tileSideLen;

MObject MayaNode::myListAttr;
MObject MayaNode::inputFile;
MObject MayaNode::maxOccurency;
MObject MayaNode::weight;

MObject MayaNode::inputGeometry;
MObject MayaNode::outputGeometry;
MObject MayaNode::tile_display;

MObject MayaNode::dropList;

std::vector<std::vector<std::vector<bool>>> tiles_storage;

//Helper Function
std::vector<std::vector<bool>> initialize_vector() {
	std::vector<bool> row(5,false);
	std::vector<std::vector<bool>> res(5,row);
	return res;
}

//the function return the path of the file
std::vector<std::string> tileFactory() {
	std::vector<std::string> v;
	//std::string folder = "E:\\GitStorage\\CIS6600\\CIS660-Authoring-Tool\\Quadrangulation\\Quadrangulation\\Quadrangulation\\tiles\\";
	std::string folder = "D:\\Study\\CIS660\\AuthoringTool\\CIS660-Authoring-Tool\\Quadrangulation\\Quadrangulation\\Quadrangulation\\tiles\\";
	v.push_back(folder + "read.txt");
	std::vector<std::string> names;
	std::ifstream myfile(v.at(0));
	if (myfile.is_open()) {
		MGlobal::displayInfo("File is opened in the tileFactory");
		std::string mystring;
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
			tiles_storage.push_back(tile);
			MGlobal::displayInfo(mystring.c_str());
		}
		for (int i = 0; i < tiles_storage.size(); i++) {
			names.push_back("tile " + std::to_string(i+1));
		}
	}
	else {
		MGlobal::displayInfo("Wrong path");	
		return std::vector<std::string>();
	}
	return names;
}

std::string generateKey(const std::string& path) {
	std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
	std::string::size_type const p(base_filename.find_last_of('.'));
	std::string file_without_extension = base_filename.substr(0, p);
	return file_without_extension;
}

std::vector<std::string> MayaNode::allTile = tileFactory();


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
		//vector store information
		std::vector<std::vector<std::vector<bool>>> all_tiles;
		std::vector<float> weights;
		std::vector<int> maxOccurs;

		// Iterate over the array elements
		//initialize all the tiles
		MArrayDataHandle arrayHandle = data.inputArrayValue(myListAttr, &returnStatus);
		for (int i = 0; i < arrayHandle.elementCount(); i++) {
			// Get the compound data handle for the current element
			MDataHandle elementHandle = arrayHandle.inputValue(&returnStatus);

			// Get the child handles for the string and number attributes
			//MDataHandle pathHandle = elementHandle.child(inputFile);
			MDataHandle weightHandle = elementHandle.child(weight);
			MDataHandle maxOccurHandle = elementHandle.child(maxOccurency);
			MDataHandle tileListHandle = elementHandle.child(dropList);
		
			//allow the program to read files
			//MString filelocation = pathHandle.asString();
			std::ifstream myfile("filelocation.asChar()");
			if (myfile.is_open()) {
				std::string mystring;
				MGlobal::displayInfo(MString("Open with file path: "));
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
				
				//we update weight and max occurency for each tile
				std::string str = "weight: " + std::to_string(weightHandle.asFloat());
				MGlobal::displayInfo(MString(str.c_str()));
				str = "max occurency: " + std::to_string(maxOccurHandle.asInt());
				MGlobal::displayInfo(MString(str.c_str()));
				weights.push_back(weightHandle.asFloat());
				maxOccurs.push_back(maxOccurHandle.asInt());
			}
			else {
				std::string str = "open file failed at: " + i;
				MGlobal::displayInfo(MString(str.c_str()));
				myfile.close();
				MGlobal::displayInfo(MString("Open with predefined tile: "));
				int enumValue = tileListHandle.asInt();
				/*myfile.open(allTile[enumValue].c_str());
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
					//we update weight and max occurency for each tile
					std::string str = "weight: " + std::to_string(weightHandle.asFloat());
					MGlobal::displayInfo(MString(str.c_str()));
					str = "max occurency: " + std::to_string(maxOccurHandle.asInt());
					MGlobal::displayInfo(MString(str.c_str()));
					weights.push_back(weightHandle.asFloat());
					maxOccurs.push_back(maxOccurHandle.asInt());
				}
				else {
					std::string str = "open file failed at: " + i;
					MGlobal::displayInfo(MString(str.c_str()));
				}*/
				if (tiles_storage.size() > 0) {
					all_tiles.push_back(tiles_storage.at(enumValue));
					std::string str = "weight: " + std::to_string(weightHandle.asFloat());
					MGlobal::displayInfo(MString(str.c_str()));
					str = "max occurency: " + std::to_string(maxOccurHandle.asInt());
					MGlobal::displayInfo(MString(str.c_str()));
					weights.push_back(weightHandle.asFloat());
					maxOccurs.push_back(maxOccurHandle.asInt());
				}
				
			}
			//myfile.close();
			// Move to the next element in the array
			arrayHandle.next();
		}

		MFnMesh mesh(oponentList.asMesh());
		MFloatPointArray pts = MFloatPointArray();
		MIntArray vertexCount = MIntArray();
		MIntArray vertexList = MIntArray();
		mesh.getPoints(pts);
		mesh.getVertices(vertexCount, vertexList);

		//side length
		MDataHandle sideHandle = data.inputValue(tileSideLen, &returnStatus);
		McheckErr(returnStatus, "Error getting side length data handle\n");
		float sideLen = std::max(sideHandle.asFloat(),0.02f);

		//construct patch
		MIntArray vertexListf = MIntArray();
		mesh.getPolygonVertices(0, vertexListf);
		std::vector<glm::vec3> verts;
		for (auto id : vertexListf) {
			verts.push_back(glm::vec3(pts[id].x, pts[id].y, pts[id].z));
		}

		Patch patch(verts);
		patch.quadrangulate(sideLen);

		std::vector<Tile*> tiles;

		for (int i = 0; i < all_tiles.size(); i++) {
			LTile* t = new LTile();
			t->box = all_tiles.at(i);
			tiles.push_back(t);
		}
		for (int i = 0;i < maxOccurs.size();i++) {
			tiles[i]->maxOccurence = maxOccurs[i];
			tiles[i]->weight = weights[i];
		}
		TileSolver solver;
		std::vector<Face*> quad;
		for (auto&& q : patch.quads) {
			quad.push_back(q.get());
		}
		
		//debug
		std::string str = "tile number: " + std::to_string(tiles.size());
		MGlobal::displayInfo(MString(str.c_str()));

		std::vector<std::vector<Face*>> result = solver.solveTiling(quad, tiles);
		if (result.size() == 0) {
			std::string str = "no solution!";
			MGlobal::displayInfo(MString(str.c_str()));
		}


		//build tile geometry in the patch
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

		//add tile display geometry into the maya

		for (int i = 0; i < all_tiles.size(); i++) {
			//each tile
			glm::vec3 origin(5, 0, (i+1)*5);
			for (int j = 0; j < all_tiles.at(i).size(); j++) {
				for (int k = 0; k < all_tiles.at(i).at(j).size(); k++) {
					if (all_tiles[i][j][k]) {
						//create a face
						int face_num = 4;
						glm::vec3 topleft = origin + glm::vec3(k, 0, j);
						glm::vec3 topright = topleft + glm::vec3(1, 0, 0);
						glm::vec3 botleft = topleft + glm::vec3(0, 0, 1);
						glm::vec3 botright = topleft + glm::vec3(1, 0, 1);
						faceCounts.append(face_num);
						mPos.append(MPoint(topleft.x, topleft.y, topleft.z));
						mPos.append(MPoint(topright.x, topright.y, topright.z));
						mPos.append(MPoint(botright.x, botright.y, botright.z));
						mPos.append(MPoint(botleft.x, botleft.y, botleft.z));
						int size = mPos.length();
						faceConnects.append(size - 1);
						faceConnects.append(size - 2);
						faceConnects.append(size - 3);
						faceConnects.append(size - 4);
					}
					else {
						break;
					}
				}
			}
		}

		//test
		int enumValue = data.inputValue(dropList,&returnStatus).asInt();
		std::string val = "value is: "+std::to_string(enumValue);
		MGlobal::displayInfo(val.c_str());

		//mesh manipulation
		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR: creating output data\n");
		MFnMesh meshFS;
		
		meshFS.create(mPos.length(), faceCounts.length(), mPos, faceCounts, faceConnects, newOutputData, &returnStatus);
		/*MColor col(1, 0, 0);
		if (faceCounts.length() > 0) { 
			meshFS.setFaceColor(col, 0);
			meshFS.setFaceColor(col, 1);
			meshFS.setFaceColor(col, 2);
			meshFS.setFaceColor(col, 3);
		}*/
		//newOutputData = meshFS.generateSmoothMesh(newOutputData,nullptr,&returnStatus);
		McheckErr(returnStatus, "ERROR: creating new geometry");

		//output handle
		MDataHandle outGeomHandle = data.outputValue(outputGeometry, &returnStatus);
		McheckErr(returnStatus, "ERROR: geometry data handle\n");
		outGeomHandle.set(newOutputData);
		//https://download.autodesk.com/us/maya/2011help/API/class_m_fn_mesh.html
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

	MStatus status;
	MFnCompoundAttribute typedAttr;
	MFnEnumAttribute dropdown;

	//set attributes
	MayaNode::myListAttr = typedAttr.create("TileAttr", "mla", &returnStatus);
	MayaNode::dropList = dropdown.create("preDefinedTile", "drop");
	for (int i = 0;i < allTile.size();i++) {
		std::string key = generateKey(allTile[i]);
		dropdown.addField(MString(key.c_str()), i);
		std::string info = "initialize list: key: " + key + ", " + std::to_string(i);
		MGlobal::displayInfo(info.c_str());
	}
	addAttribute(dropList);
	typedAttr.addChild(dropList);

	//test
	//typedAttr.addChild(dropList);

	typedAttr.setArray(true);
	typedAttr.setUsesArrayDataBuilder(true);

	//set input file location
	/*MFnTypedAttribute stringAttr;
	MayaNode::inputFile = stringAttr.create("InputFile", "inputfile", MFnData::kString, MObject::kNullObj, &returnStatus);
	McheckErr(returnStatus, "Error creating input file attribte\n");
	typedAttr.addChild(inputFile);*/

	MFnNumericAttribute numericAttr;
	//set occurrencies
	MayaNode::maxOccurency = numericAttr.create("MaxOccurency", "max_occur_1", MFnNumericData::kInt, -1, &returnStatus);
	McheckErr(returnStatus, "Error creating node max_occurency_1 attribute\n");
	typedAttr.addChild(maxOccurency);
	MayaNode::weight = numericAttr.create("Weight", "w_1", MFnNumericData::kFloat, 0.f, &returnStatus);
	McheckErr(returnStatus, "Error creating node weight_1 attribute\n");
	typedAttr.addChild(weight);

	MayaNode::tileSideLen = weightAttr.create("tile_length", "tile_len", MFnNumericData::kFloat, 0.2f, &returnStatus);
	McheckErr(returnStatus, "Error creating node tile_length attribute\n");
	MayaNode::inputGeometry = geomAttr.create("input_geometry", "i_geom", MFnData::kMesh, MObject::kNullObj, &returnStatus);
	McheckErr(returnStatus, "Error creating input_geometry attribute\n");
	MayaNode::outputGeometry = geomAttr.create("output_geometry", "o_geom", MFnData::kMesh, MObject::kNullObj, &returnStatus);
	McheckErr(returnStatus, "Error creating output_geometry attribute\n");

	//add attributes
	returnStatus = addAttribute(MayaNode::myListAttr);
	McheckErr(returnStatus, "Error adding myListAttr attribute");

	returnStatus = addAttribute(MayaNode::tileSideLen);
	McheckErr(returnStatus, "Error adding tile_length attribute");
	
	returnStatus = addAttribute(MayaNode::inputGeometry);
	McheckErr(returnStatus, "Error adding input geometry attribute");
	if (returnStatus == MS::kSuccess) {
		MFnAttribute myAttrFn(MayaNode::inputGeometry, &returnStatus);
		if (returnStatus == MS::kSuccess) {
			myAttrFn.setHidden(true);
		}
	}
	
	returnStatus = addAttribute(MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding output geometry attribute");
	if (returnStatus == MS::kSuccess) {
		MFnAttribute myAttrFn(MayaNode::outputGeometry, &returnStatus);
		if (returnStatus == MS::kSuccess) {
			myAttrFn.setHidden(true);
		}
	}

	//attributes affect
	//returnStatus = attributeAffects(MayaNode::inputFile, MayaNode::outputGeometry);
	//McheckErr(returnStatus, "Error adding input file attributeAffect");
	returnStatus = attributeAffects(MayaNode::tileSideLen, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding tileSideLen attributeAffect");
	returnStatus = attributeAffects(MayaNode::myListAttr, MayaNode::outputGeometry);
	McheckErr(returnStatus, "Error adding input file attributeAffect");
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