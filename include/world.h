#ifndef WORLD_H
#define WORLD_H

class Player;
class Monster;

class World {
public:
	char terrainmesh[TOWER_HEIGHT][60][60];
	int itemmesh[TOWER_HEIGHT][60][60];
	Player* player;
	vector<Monster> monsters;
	void initialize();
	void read_world();
	void copy_level(ifstream *levelfile, int targetfloor);
	void skip_level(ifstream *levelfile);
	string skip_whitespace(ifstream *levelfile);
	bool is_space_free(int currentfloor, int xpos, int ypos);
	bool is_vision_blocking(int currentfloor, int xpos, int ypos);
	bool is_monster(int currentfloor, int xpos, int ypos);
	bool is_door(int currentfloor, int xpos, int ypos);
	bool open_door(int currentfloor, int xpos, int ypos);
	Monster* get_monster(int currentfloor, int xpos, int ypos);
	void place_monsters();
	void monsters_turn();
	void dust_up();

};

#endif
