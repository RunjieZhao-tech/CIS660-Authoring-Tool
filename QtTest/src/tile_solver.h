#pragma once
#include"lp_solve/lp_lib.h"
#include "scene/halfedge.h"
#include <iostream>
#pragma comment(lib, "lp_solve/lpsolve55.lib")
class Tile {
public:
	float weight;
    Tile(float w):weight(w){};
    Tile():Tile(0) {};
    virtual ~Tile();
	//helper function
	bool hasTiled(const Face* QuadWantTile, const std::vector<Face*>& tiledFace)const;
	//iterate all possible filing for a certain tile
    virtual std::vector<std::vector<Face*>> possibleTile(std::vector<Face*>& quads)const=0;
};

//tile[]
//    []
//    [][]
class LTile: public Tile {
public:
    LTile():Tile(4) {};
	std::vector<std::vector<Face*>> possibleTile(std::vector<Face*>& quads)const override;
};

//tile[][]
//    [][]
class SquareTile : public Tile {
public:
    SquareTile():Tile(4) {};
	std::vector<std::vector<Face*>> possibleTile(std::vector<Face*>& quads)const override;
};

//tile: [][][]
class L3Tile : public Tile {
public:
    L3Tile():Tile(3) {};
	std::vector<std::vector<Face*>> possibleTile(std::vector<Face*>& quads)const override;
};

//tile: [][]
class L2Tile : public Tile {
public:
    L2Tile():Tile(2) {};
	std::vector<std::vector<Face*>> possibleTile(std::vector<Face*>& quads)const override;
};

class TileSolver {
public:
    TileSolver();
    std::vector<std::vector<Face*>> solveTiling(std::vector<Face*>& quads, std::vector<Tile*>tiles)const;
};
