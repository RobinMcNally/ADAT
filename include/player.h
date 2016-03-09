#ifndef PLAYER_H
#define PLAYER_H
class Player {
public:
	int health;
	int maxhealth;
	int stamina;
	int maxstamina;
	bool hasmoved;
	int xlocation;
	int ylocation;
	int currentfloor;
	char world[TOWER_HEIGHT][60][60];
	int color_mesh[60][60];
	void initialize_player_world();
	void clear_color_mesh();
};
#endif
