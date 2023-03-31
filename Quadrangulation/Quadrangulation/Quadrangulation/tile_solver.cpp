#include "tile_solver.h"

bool Tile::hasTiled(const Face* QuadWantTile, const std::vector<Face*>& tiledFace)const {
	for (auto tile : tiledFace) {
		if (tile == QuadWantTile)return true;
	}
	return false;
}

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
	lp = make_lp(quads.size(), 0);
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
	std::vector<double> rowVal;
	rowVal.reserve(totalColNum + 1);//allocate memory
	
	for (int i = 0;i < quads.size();i++) {
		rowVal.erase(rowVal.begin(), rowVal.begin()+ totalColNum + 1);
		get_row(lp, i + 1, &rowVal[0]);
		add_constraint(lp, &rowVal[0], /*EQ */LE, 1);
	}
	//rowId      colVal
	//   v       v
	//   v T1 T2 T3 ...
	//  0| 4  1  3  ...  << row value
	//  1| 1  1  1  ...
	//  2| 0  1  0  ...
	// ..|
	//we want to maximize sum(W*T)
	rowVal.erase(rowVal.begin(), rowVal.begin() + totalColNum + 1);
	get_row(lp, 0, &rowVal[0]);
	set_obj_fn(lp, &rowVal[0]);
	set_maxim(lp);
	solve(lp);
	get_variables(lp, &rowVal[0]);//get result tiling for T1, T2 ...
	std::vector<std::vector<Face*>> result;
	//allocate memory
	std::vector<int> rowId;
	rowId.reserve(quads.size() + 1);
	std::vector<double> colValue;
	colValue.reserve(quads.size() + 1);//actually useless since it's a binary value
	//find valid tile choice convert it to vector<Face*> used in result
	for (int i = 0;i < totalColNum;i++) {
		if (rowVal[i] == 0)continue;//this tiling choice is not selected;
		//clean vector to store new information
		rowId.erase(rowId.begin(), rowId.begin() + quads.size() + 1);
		colValue.erase(colValue.begin(), colValue.begin() + quads.size() + 1);
		
		//for each col find the tiledface described by the column
		std::vector<Face*> tiledFace;
		get_columnex(lp, i + 1, &colValue[0], &rowId[0]);
		for (int i = 0;i < quads.size() + 1;i++) {
			if (rowId[i] == 0) {
				if (colValue[i] == 0)break;//uninitialized useless value
				continue;//objective row
			}
			Face* tiledQuad = quads[rowId[i] - 1];//because we store quad id begins with 1
			tiledFace.push_back(tiledQuad);
		}
		result.push_back(tiledFace);
	}
	return result;
}
//int demo()
//{
//    lprec* lp;
//    int Ncol, * colno = NULL, j, ret = 0;
//    REAL* row = NULL;
//
//    /* We will build the model row by row
//       So we start with creating a model with 0 rows and 2 columns */
//    Ncol = 2; /* there are two variables in the model */
//    lp = make_lp(0, Ncol);
//    if (lp == NULL)
//        ret = 1; /* couldn't construct a new model... */
//
//    if (ret == 0) {
//        /* let us name our variables. Not required, but can be useful for debugging */
//        set_col_name(lp, 1, "x");
//        set_col_name(lp, 2, "y");
//
//        /* create space large enough for one row */
//        colno = (int*)malloc(Ncol * sizeof(*colno));
//        row = (REAL*)malloc(Ncol * sizeof(*row));
//        if ((colno == NULL) || (row == NULL))
//            ret = 2;
//    }
//
//    if (ret == 0) {
//        set_add_rowmode(lp, TRUE);  /* makes building the model faster if it is done rows by row */
//
//        /* construct first row (120 x + 210 y <= 15000) */
//        j = 0;
//
//        colno[j] = 1; /* first column */
//        row[j++] = 120;
//
//        colno[j] = 2; /* second column */
//        row[j++] = 210;
//
//        /* add the row to lpsolve */
//        if (!add_constraintex(lp, j, row, colno, LE, 15000))
//            ret = 3;
//    }
//
//    if (ret == 0) {
//        /* construct second row (110 x + 30 y <= 4000) */
//        j = 0;
//
//        colno[j] = 1; /* first column */
//        row[j++] = 110;
//
//        colno[j] = 2; /* second column */
//        row[j++] = 30;
//
//        /* add the row to lpsolve */
//        if (!add_constraintex(lp, j, row, colno, LE, 4000))
//            ret = 3;
//    }
//
//    if (ret == 0) {
//        /* construct third row (x + y <= 75) */
//        j = 0;
//
//        colno[j] = 1; /* first column */
//        row[j++] = 1;
//
//        colno[j] = 2; /* second column */
//        row[j++] = 1;
//
//        /* add the row to lpsolve */
//        if (!add_constraintex(lp, j, row, colno, LE, 75))
//            ret = 3;
//    }
//
//    if (ret == 0) {
//        set_add_rowmode(lp, FALSE); /* rowmode should be turned off again when done building the model */
//
//        /* set the objective function (143 x + 60 y) */
//        j = 0;
//
//        colno[j] = 1; /* first column */
//        row[j++] = 143;
//
//        colno[j] = 2; /* second column */
//        row[j++] = 60;
//
//        /* set the objective in lpsolve */
//        if (!set_obj_fnex(lp, j, row, colno))
//            ret = 4;
//    }
//
//    if (ret == 0) {
//        /* set the object direction to maximize */
//        set_maxim(lp);
//
//        /* just out of curioucity, now show the model in lp format on screen */
//        /* this only works if this is a console application. If not, use write_lp and a filename */
//        write_LP(lp, stdout);
//        /* write_lp(lp, "model.lp"); */
//
//        /* I only want to see important messages on screen while solving */
//        set_verbose(lp, IMPORTANT);
//
//        /* Now let lpsolve calculate a solution */
//        ret = solve(lp);
//        if (ret == OPTIMAL)
//            ret = 0;
//        else
//            ret = 5;
//    }
//
//    if (ret == 0) {
//        /* a solution is calculated, now lets get some results */
//
//        /* objective value */
//        printf("Objective value: %f\n", get_objective(lp));
//
//        /* variable values */
//        get_variables(lp, row);
//        for (j = 0; j < Ncol; j++)
//            printf("%s: %f\n", get_col_name(lp, j + 1), row[j]);
//
//        /* we are done now */
//    }
//
//    /* free allocated memory */
//    if (row != NULL)
//        free(row);
//    if (colno != NULL)
//        free(colno);
//
//    if (lp != NULL) {
//        /* clean up such that all used memory by lpsolve is freed */
//        delete_lp(lp);
//    }
//
//    return(ret);
//}
//
//int main()
//{
//    demo();
//}