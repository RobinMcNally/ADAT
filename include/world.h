#ifndef WORLD_H
#define WORLD_H

using namespace std;

class World {
public:
	char terrainmesh[TOWER_HEIGHT][60][60];
	int itemmesh[TOWER_HEIGHT][60][60];
	Player player;
	vector<Monster> monsters;
	void initialize();
	void read_world();
	void copy_level(ifstream *levelfile, int targetfloor);
	void skip_level(ifstream *levelfile);
	string skip_whitespace(ifstream *levelfile);
	bool is_space_free(int currentfloor, int xpos, int ypos);
	void place_monsters();
	void monsters_turn();
};

#endif
