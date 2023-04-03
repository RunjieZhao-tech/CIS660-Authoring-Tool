#include "tile_solver.h"

bool Tile::hasTiled(const Face* QuadWantTile, const std::vector<Face*>& tiledFace)const {
	for (auto tile : tiledFace) {
		if (tile == QuadWantTile)return true;
	}
    return false;
}

Tile::~Tile(){}

//tile[3]
//    [2]
//    [1][0]
std::vector<std::vector<Face*>> LTile::possibleTile(std::vector<Face*>& quads)const {
	std::vector<std::vector<Face*>> result;
	for (auto quad : quads) {
		std::vector<HalfEdge*> halfEdges = quad->getHalfEdges();
		for (auto halfEdge : halfEdges) {
			std::vector<Face*> possibleTile;
			//0
			possibleTile.push_back(quad);
			HalfEdge* sym = halfEdge->getSymHalfEdge();
			Face* nextQuad = sym->getFace();
			if (nextQuad == nullptr)continue;
			//1
			if (hasTiled(nextQuad, possibleTile))continue;
			possibleTile.push_back(nextQuad);
			sym = sym->getNextHalfEdge()->getSymHalfEdge();
			nextQuad = sym->getFace();
			if (nextQuad == nullptr || sym == nullptr)continue;
			//2
			if (hasTiled(nextQuad, possibleTile))continue;
			possibleTile.push_back(nextQuad);
			Face* rightFace = sym->getNextHalfEdge()->getSymHalfEdge()->getFace();
			if (hasTiled(rightFace, possibleTile))continue;//to avoid vertex that has only 3 adjacent faces
			sym = sym->getNextHalfEdge()->getNextHalfEdge()->getSymHalfEdge();
			nextQuad = sym->getFace();
			if (nextQuad == nullptr || sym == nullptr)continue;
			//3
			if (hasTiled(nextQuad, possibleTile))continue;
			possibleTile.push_back(nextQuad);

			result.push_back(possibleTile);
		}
		
	}
	return result;
}
//tile[1][2]
//    [0][3]
std::vector<std::vector<Face*>> SquareTile::possibleTile(std::vector<Face*>& quads)const {
	std::vector<std::vector<Face*>> result;
	for (auto quad : quads) {
		std::vector<HalfEdge*> halfEdges = quad->getHalfEdges();
		for (auto halfEdge : halfEdges) {
			std::vector<Face*> possibleTile;
			//0
			possibleTile.push_back(quad);
			HalfEdge* sym = halfEdge->getSymHalfEdge();
			Face* nextQuad = sym->getFace();
			if (nextQuad == nullptr)continue;
			//1
			if (hasTiled(nextQuad, possibleTile))continue;
			possibleTile.push_back(nextQuad);
			sym = sym->getNextHalfEdge()->getSymHalfEdge();
			nextQuad = sym->getFace();
			if (nextQuad == nullptr || sym == nullptr)continue;
			//2
			if (hasTiled(nextQuad, possibleTile))continue;
			possibleTile.push_back(nextQuad);
			sym = sym->getNextHalfEdge()->getSymHalfEdge();
			nextQuad = sym->getFace();
			if (nextQuad == nullptr || sym == nullptr)continue;
			//3
			if (hasTiled(nextQuad, possibleTile))continue;
			possibleTile.push_back(nextQuad);

			result.push_back(possibleTile);
		}

	}
	return result;
}
//tile: [0][1][2]
std::vector<std::vector<Face*>> L3Tile::possibleTile(std::vector<Face*>& quads)const {
	std::vector<std::vector<Face*>> result;
	for (auto quad : quads) {
		std::vector<HalfEdge*> halfEdges = quad->getHalfEdges();
		for (auto halfEdge : halfEdges) {
			std::vector<Face*> possibleTile;
			//0
			possibleTile.push_back(quad);
			HalfEdge* sym = halfEdge->getSymHalfEdge();
			Face* nextQuad = sym->getFace();
			if (nextQuad == nullptr)continue;
			//1
			if (hasTiled(nextQuad, possibleTile))continue;
			possibleTile.push_back(nextQuad);
			sym = sym->getNextHalfEdge()->getNextHalfEdge()->getSymHalfEdge();
			nextQuad = sym->getFace();
			if (nextQuad == nullptr || sym == nullptr)continue;
			//2
			if (hasTiled(nextQuad, possibleTile))continue;
			possibleTile.push_back(nextQuad);

			result.push_back(possibleTile);
		}

	}
	return result;
}
//tile: [0][1]
std::vector<std::vector<Face*>> L2Tile::possibleTile(std::vector<Face*>& quads)const {
	std::vector<std::vector<Face*>> result;
	for (auto quad : quads) {
		std::vector<HalfEdge*> halfEdges = quad->getHalfEdges();
		for (auto halfEdge : halfEdges) {
			std::vector<Face*> possibleTile;
			//0
			possibleTile.push_back(quad);
			HalfEdge* sym = halfEdge->getSymHalfEdge();
			Face* nextQuad = sym->getFace();
			if (nextQuad == nullptr)continue;
			//1
			if (hasTiled(nextQuad, possibleTile))continue;
			possibleTile.push_back(nextQuad);

			result.push_back(possibleTile);
		}

	}
	return result;
}

TileSolver::TileSolver(){}

std::vector<std::vector<Face*>> TileSolver::solveTiling(std::vector<Face*>& quads, std::vector<Tile*>tiles)const {
	lprec* lp;
	int Ncol = 0;//number of all possible tiling
	int* T = nullptr;
	REAL* weights = nullptr;

	//build a map refer to all quads in a patch
	std::unordered_map<Face*, int> quadmap;
	for (int i = 0;i < quads.size();i++) {
		quadmap[quads[i]] = i + 1;
	}
	//build the linear problem solver
    lp = make_lp(quads.size(),0);
	if (lp == NULL) {
		std::cout << "make linear solver failed" << std::endl;
	}

	//for each tiles, find its possible tiling and build a column for it
	int colIndex = 0;//to find which col is currently set
    for (Tile* tile : tiles) {
		std::vector<std::vector<Face*>> tileChoice = tile->possibleTile(quads);
		for (int i = 0;i < tileChoice.size();i++) {
			std::vector<int> rowNumber; // row that has T for this tiling choice
			//for the maximize function
			rowNumber.push_back(0);//the objective function is always the first row
			std::vector<double> colFactor;
			colFactor.push_back(tile->weight);
			//describe quads(fk in the paper) that is going to be filled with T
			//this is used for constraint sum(T) <= 1
			for (int j = 0;j < tileChoice[i].size();j++) {
				//find index of quad
				auto itr = quadmap.find((tileChoice[i][j]));
				rowNumber.push_back(itr->second);//describe which face is affected
				colFactor.push_back(1);//since sum(T) is what we want, to make sure the size of colFactor and rowNumber stay the same
			}
			add_columnex(lp, rowNumber.size(), &colFactor[0], &rowNumber[0]);
            set_binary(lp, colIndex+1, TRUE);//tell lpsolver T is binary
			colIndex++;
		}
	}

	//for each fk in M, sum(T) should be 1;
	int totalColNum = get_Ncolumns(lp);
    REAL* rowVal = nullptr;
    rowVal = (REAL*)malloc((totalColNum)*sizeof(*rowVal));//allocate memory
    for (int i = 0;i < quads.size();i++) {
        get_row(lp, 1, rowVal);
        add_constraint(lp, rowVal, /*EQ */LE, 1);
        del_constraint(lp,1);
    }
	//rowId      colVal
	//   v       v
	//   v T1 T2 T3 ...
	//  0| 4  1  3  ...  << row value
	//  1| 1  1  1  ...
	//  2| 0  1  0  ...
	// ..|
	//we want to maximize sum(W*T)
    get_row(lp, 0, rowVal);
    set_obj_fn(lp, rowVal);
	set_maxim(lp);
	solve(lp);
    get_variables(lp, rowVal);//get result tiling for T1, T2 ...
	std::vector<std::vector<Face*>> result;
	//allocate memory
    //int* rowId = nullptr;
    //rowId = (int*)malloc((quads.size() + 1)*sizeof(*rowId));
    REAL* colValue = nullptr;
    colValue = (REAL*)malloc((quads.size() + 1)*sizeof(*colValue));//actually useless since it's a binary value

	//find valid tile choice convert it to vector<Face*> used in result
	for (int i = 0;i < totalColNum;i++) {
		if (rowVal[i] == 0)continue;//this tiling choice is not selected;
		
		//for each col find the tiledface described by the column
		std::vector<Face*> tiledFace;
        get_column(lp, i + 1, colValue);
        for (int i = 1;i < quads.size() + 1;i++) {
            if (colValue[i] == 0)continue;//not tiled face
            Face* tiledQuad = quads[i - 1];//because we store quad id begins with 1
			tiledFace.push_back(tiledQuad);
		}
		result.push_back(tiledFace);
	}
    if (lp != NULL) {
        /* clean up such that all used memory by lpsolve is freed */
        delete_lp(lp);
    }
    free(rowVal);
    free(colValue);

	return result;
}
