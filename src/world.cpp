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
				monstermesh[i][j][k] = 0;
			}
		}
	}
	for (size_t i = 0; i < TOWER_HEIGHT; i++) {
		for (size_t j = 0; j < 60; j++) {
			for (size_t k = 0; k < 60; k++) {
				itemmesh[i][j][k] = 0;
			}
		}
	}
	place_monsters();
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
		cout << "hi\n";
	} else {
		cout << "Levels file failed to open. Exiting.\n";
	}
}

void World::place_monsters() {
	srand(time(NULL));
	int x;
	int y;
	set<char> chars;
	chars.insert(IMPASSIBLE_TERRAIN, IMPASSIBLE_TERRAIN + strlen(IMPASSIBLE_TERRAIN));
	bool placed = false;
	MonsterFactory monst;

	for (size_t i = 0; i < MONSTER_COUNT; i++) {
		placed = false;
		while (!placed) {
			x = rand() % 60;
			y = rand() % 60;
			if (chars.find(terrainmesh[0][x][y]) != chars.end()) {
				monsters.push_back(monst.make_monster());
				monsters.back().xlocation = x;
				monsters.back().ylocation = y;
				placed = true;
			}
		}
	}
}
