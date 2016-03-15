#include "../include/includes.h"

void World::initialize() {
	for (size_t i = 0; i < TOWER_HEIGHT; i++) {
		for (size_t j = 0; j < 60; j++) {
			for (size_t k = 0; k < 60; k++) {
				terrainmesh[i][j][k] = '=';
			}
		}
	}
	read_world();
	for (size_t i = 0; i < TOWER_HEIGHT; i++) {
		for (size_t j = 0; j < 60; j++) {
			for (size_t k = 0; k < 60; k++) {
				itemmesh[i][j][k] = 0;
			}
		}
	}
	place_monsters();
	player = new Player();
	player->xlocation = 14;
	player->ylocation = 14;
	player->hasmoved = false;
	player->currentfloor = 0;
}

/* Subfunction for read_world
 *
 * This takes the file stream that we are working on and imports the
 * floor that the stream points at to the floor that targetfloor indicates
 */
void World::copy_level(ifstream *levelfile, int targetfloor) {
	string line;

	for (int i = 0; i < SCREEN_RADIUS * 2; i++) {
		getline (*levelfile, line);
		for (size_t j = 0; j < line.size(); j++) {
			terrainmesh[targetfloor][j][i] = line[j];
		}
	}
}

/* Subfunction for read_world
 *
 * This function is called at the beginning of an undesired level in the
 * levels file. It will bypass the level
 */
void World::skip_level(ifstream *levelfile) {
	string line;

	for (int i = 0; i < SCREEN_RADIUS * 2; i++) {
		getline (*levelfile, line);
	}
}

/* Subfunction for read_world
 *
 * This skips any whitespace (and comment lines which begine with *)
 */
string World::skip_whitespace(ifstream *levelfile) {
	string line = "";
	while (line == "") {
		getline(*levelfile, line);
		if (line[0] == '*') {
			line = "";
		}
	}
	return line;
}

/* Function that reads through the level text file and generates a Tower
 * based on that file.
 *
 * This world generation is not random. Only the order of the levels is.
 */
void World::read_world() {
	string line;
	int floorcount;
	int randomfloor = 0;
	int parsedcount = 0;
	int currentfloor = 0;
	srand(time(NULL));

	ifstream levelfile("res/levels.txt");
	if (levelfile.is_open()) {
		line = skip_whitespace(&levelfile);

		//Verify that the line is only an integer
		for (size_t i = 0; i < line.size(); i++) {
			if (!isdigit(line[0])) {
				cout << "Improper syntax in levels file\n" << line;
				exit(1);
			}
		}
		floorcount = stoi(line);

		for (int i = 0; i < TOWER_HEIGHT; i++) {
			randomfloor = rand() % floorcount;
			while (parsedcount < floorcount) {
				//Scan through blank lines (or ones with comments) till we get one with something in it
				line = skip_whitespace(&levelfile);
				for (size_t i = 0; i < line.size(); i++) {
					if (!isdigit(line[0])) {
						cout << "Improper syntax in levels file\n" << line;
						exit(1);
					}
				}
				if (stoi(line) == randomfloor) {
					copy_level(&levelfile, currentfloor);
				} else {
					skip_level(&levelfile);
				}
				parsedcount++;
			}
			levelfile.clear();
			levelfile.seekg(0, ios::beg);
			getline(levelfile, line);
		}
		levelfile.close();
	} else {
		cout << "Levels file failed to open. Exiting.\n";
	}
}

void World::place_monsters() {
	MonsterFactory monst;

	for (size_t i = 0; i < TOWER_HEIGHT; i++) {
		for (size_t j = 0; j < 60; j++) {
			for (size_t k = 0; k < 60; k++) {
				if (terrainmesh[i][j][k] == 'm') {
					monsters.push_back(monst.make_monster());
					monsters.back().xlocation = j;
					monsters.back().ylocation = k;
				}
			}
		}
	}
}

bool World::is_vision_blocking(int currentfloor, int xpos, int ypos) {
	set<char> chars;
	chars.insert(IMPASSIBLE_TERRAIN, IMPASSIBLE_TERRAIN + strlen(IMPASSIBLE_TERRAIN));

	bool visionblocked = (chars.find(terrainmesh[currentfloor][xpos][ypos]) != chars.end());

	return visionblocked;
}

bool World::is_space_free(int currentfloor, int xpos, int ypos) {
	set<char> chars;
	chars.insert(IMPASSIBLE_TERRAIN, IMPASSIBLE_TERRAIN + strlen(IMPASSIBLE_TERRAIN));

	bool terrainfree = (chars.find(terrainmesh[currentfloor][xpos][ypos]) == chars.end());
	bool monstersfree = !is_monster(currentfloor, xpos, ypos);
	bool playerfree = !(player->xlocation == xpos && player->ylocation == ypos);

	return (terrainfree && monstersfree && playerfree);
}

void World::monsters_turn() {
	int offset[2] = {0};

	for (vector<Monster>::iterator it = monsters.begin(); it != monsters.end(); it++) {
		//Only move if the player can see us for now
		//Also, this is the worst approach to pathfinding ever and I need to
		//replace it
		if (player->color_mesh[it->xlocation][it->ylocation] == 1) {
			offset[0] = sign(player->xlocation - it->xlocation);
			offset[1] = sign(player->ylocation - it->ylocation);

			if (is_space_free(it->currentfloor, it->xlocation + offset[0], it->ylocation + offset[1])) {
				it->xlocation += offset[0];
				it->ylocation += offset[1];
			}
		}
	}
}

void World::dust_up() {
	for (vector<Monster>::iterator it = monsters.begin(); it != monsters.end(); it++) {
		if (it->health < 0) {
			ostringstream oss;
			oss << "The " << it->name << " has died!";
			add_to_log(oss.str());
			monsters.erase(it);
		}
	}
}

bool World::is_monster(int currentfloor, int xpos, int ypos) {
	bool ismonster = false;
	for (vector<Monster>::iterator it = monsters.begin(); it != monsters.end(); it++) {
		if (it->xlocation == xpos && it->ylocation == ypos && it->currentfloor == currentfloor) ismonster = true;
	}
	return ismonster;
}

bool World::is_door(int currentfloor, int xpos, int ypos) {
	bool isdoor = false;
	if (terrainmesh[currentfloor][xpos][ypos] == '+') isdoor = true;
	return isdoor;
}

bool World::open_door(int currentfloor, int xpos, int ypos) {
	bool completed;
	if (is_door(currentfloor, xpos, ypos)) {
		terrainmesh[currentfloor][xpos][ypos] = ',';
		completed = true;
	} else {
		completed = false;
	}
	return completed;
}

Monster* World::get_monster(int currentfloor, int xpos, int ypos) {
	for (vector<Monster>::iterator it = monsters.begin(); it != monsters.end(); it++) {
		if (it->xlocation == xpos && it->ylocation == ypos && it->currentfloor == currentfloor) {
			return &(*it);
		}
	}
	return NULL;
}
