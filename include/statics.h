
#ifndef STATICS_H
#define STATICS_H

int SCREEN_WIDTH = 1120;
int SCREEN_HEIGHT = 840;
//                           {x, y}
int SCREEN_START[2] = {12, 1};
int SCREEN_END[2] = {58, 34};
int SCREEN_CENTER[2] = {(SCREEN_END[0] - SCREEN_START[0]) / 2, (SCREEN_END[1] - SCREEN_START[1]) / 2};
int SCREEN_RADIUS = 30;
int FOV_RADIUS = 5;

int TILE_SIZE = 14;

int LOG_LINE_LENGTH = 18;
int LOG_LINE_COUNT = 18;
int LOG_LOWER_LEFT[2] = {61, 58};


int UPPER_CHAR_START = 0;
int LOWER_CHAR_START = 364;

#endif
