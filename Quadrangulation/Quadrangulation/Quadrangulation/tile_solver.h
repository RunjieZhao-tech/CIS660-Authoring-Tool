#pragma once
#include"lp_solve/lp_lib.h"
#include "half_edge.h"
#include <iostream>
#pragma comment(lib, "lp_solve/lpsolve55.lib")
class Tile {
public:
	float weight = 4;
	Tile() {};
	//iterate all possible filing for a certain tile
	virtual std::vector<std::vector<Face*>> possibleTile(const std::vector<Face*>& quads)const;
};

//tile[]
//    []
//    [][]
class LTile: public Tile {
	std::vector<std::vector<Face*>> possibleTile(const std::vector<Face*>& quads)const override;
};

//tile[][]
//    [][]
class SquareTile : public Tile {
	std::vector<std::vector<Face*>> possibleTile(const std::vector<Face*>& quads)const override;
};

//tile: [][][]
class L3Tile : public Tile {
	std::vector<std::vector<Face*>> possibleTile(const std::vector<Face*>& quads)const override;
};

//tile: [][]
class L2Tile : public Tile {
	std::vector<std::vector<Face*>> possibleTile(const std::vector<Face*>& quads)const override;
};

class TileSolver {
public:
	TileSolver();
	std::vector<std::vector<Face*>> solveTiling(const std::vector<Face*>& quads, std::vector<Tile*>tiles)const;
};