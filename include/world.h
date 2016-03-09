#ifndef WORLD_H
#define WORLD_H

using namespace std;

class World {
public:
	char terrainmesh[TOWER_HEIGHT][60][60];
	int itemmesh[TOWER_HEIGHT][60][60];
	vector<Monster> monsters;
	void initialize();
	void read_world();
	void copy_level(ifstream *levelfile, int targetfloor);
	void skip_level(ifstream *levelfile);
	string skip_whitespace(ifstream *levelfile);
	void place_monsters();
	void monsters_turn(Player *player);
};

#endif
