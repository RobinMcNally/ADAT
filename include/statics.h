
#ifndef STATICS_H
#define STATICS_H

static const int SCREEN_WIDTH = 1120;
static const int SCREEN_HEIGHT = 840;
//                           {x, y}
static const int SCREEN_START[2] = {0, 0};
static const int SCREEN_END[2] = {59, 59};
static const int SCREEN_CENTER[2] = {(SCREEN_END[0] - SCREEN_START[0]) / 2, (SCREEN_END[1] - SCREEN_START[1]) / 2};
static const int SCREEN_RADIUS = 30;
static const int FOV_RADIUS = 5;

static const int TILE_SIZE = 14;

static const int LOG_LINE_LENGTH = 18;
static const int LOG_LINE_COUNT = 18;
static const int LOG_LOWER_LEFT[2] = {61, 58};

static const int UPPER_CHAR_START = 0;
static const int LOWER_CHAR_START = 364;

static const int DARK_SPRITE_Y_START = 56;
static const int LIGHT_SPRITE_Y_START = 70;

static const int WALL_SPRITE_OFFSET = 0;
static const int FLOOR_SPRITE_OFFSET = 14;
static const int DOOR_SPRITE_OFFSET = 28;
static const int OPEN_DOOR_SPRITE_OFFSET = 98;

static const int MONSTER_COUNT = 10;
static const int MONSTER_TYPE_COUNT = 1;

static const int TOWER_HEIGHT = 6;

static const int PLAYER_START_HEALTH = 10;
static const int PLAYER_START_STAMINA = 5;

static const int PLAYER_START_STATS = 5;

static char const* IMPASSIBLE_TERRAIN = "=#+";


enum monsters : int {
	GOBLIN = 0
};

enum directions {
	NORTH = 8,
	SOUTH = 2,
	EAST = 6,
	WEST = 4,
	NE = 9,
	NW = 7,
	SE = 3,
	SW = 1,
	STILL = 5
};

#endif
