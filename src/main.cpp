#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <set>
#include <SDL2/SDL.h>
#include <unistd.h>
#include "../include/cleanup.h"
#include "../include/statics.h"

using namespace std;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int SCREEN_CENTER[2];
extern int FOV_RADIUS;
extern int TILE_SIZE;

extern int LOG_LINE_LENGTH;
extern int LOG_LINE_COUNT;
extern int LOG_LOWER_LEFT[2];

extern int UPPER_CHAR_START;
extern int LOWER_CHAR_START;

extern int DARK_SPRITE_Y_START;
extern int LIGHT_SPRITE_Y_START;

extern int WALL_SPRITE_OFFSET;
extern int FLOOR_SPRITE_OFFSET;
extern int DOOR_SPRITE_OFFSET;

extern int TOWER_HEIGHT;

char const * IMPASSIBLE_TERRAIN = "=#";

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

vector<string> logoutput;

//Notation for this file is [z][x][y] or [x][y] for two dimensional arrays
//Tower height is 6
char gameworld[6][60][60];

class Player {
public:
	bool hasmoved;
	int xlocation;
	int ylocation;
	int currentfloor;
	char world[6][60][60];
	int color_mesh[60][60];
	void initialize_player_world();
	void clear_color_mesh();
};

// Initialize the player world to the default nonrendered character ('=')
void Player::initialize_player_world() {
	for (size_t i = 0; i < 6; i++) {
		for (size_t j = 0; j < 60; j++) {
			for (size_t k = 0; k < 60; k++) {
				world[i][j][k] = '=';
			}
		}
	}
}

// Set the color mesh to zero
// This will be called each move so we should probably streamline it
// O(n^2) is a bit much for each time
void Player::clear_color_mesh() {
	for (size_t i = 0; i < 60; i++) {
		for (size_t j = 0; j < 60; j++) {
			color_mesh[i][j] = 0;
		}
	}
}

// An assertation function that fails on a null
void assertptr(void *val, std::string ErrorText) {
	if (val == nullptr) {
		cout << ErrorText << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
}

/* Subfunction for read_world
 *
 * This takes the file stream that we are working on and imports the
 * floor that the stream points at to the floor that targetfloor indicates
 */
void copy_level(ifstream *levelfile, int targetfloor) {
	string line;

	for (int i = 0; i < SCREEN_RADIUS * 2; i++) {
		getline (*levelfile, line);
		for (size_t j = 0; j < line.size(); j++) {
			gameworld[targetfloor][j][i] = line[j];
		}
	}
}

/* Subfunction for read_world
 *
 * 
 */
void skip_level(ifstream *levelfile) {
	string line;

	for (int i = 0; i < SCREEN_RADIUS * 2; i++) {
		getline (*levelfile, line);
	}
}

string skip_whitespace(ifstream *levelfile) {
	string line = "";
	while (line == "") {
		getline(*levelfile, line);
		if (line[0] == '*') {
			line = "";
		}
	}
	return line;
}

void read_world() {
	string line;
	int floorcount;
	int randomfloor = 0;
	int parsedcount = 0;
	int currentfloor = 0;

	ifstream levelfile("res/levels.txt");
	if (levelfile.is_open()) {
		line = skip_whitespace(&levelfile);

		//Super jank to the rescue
		floorcount = stoi(line);
		srand(time(NULL));
		for (int i = 0; i < TOWER_HEIGHT; i++) {
			randomfloor = rand() % floorcount;
			while (parsedcount < floorcount) {
				//Scan through blank lines (or ones with comments) till we get one with something in it
				line = skip_whitespace(&levelfile);
				for (size_t i = 0; i < line.size(); i++) {
					if (!isdigit(line[0])) {
						cout << "Improper syntax in levels file\n" << line;
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
	}
}

void logSDLError(std::ostream &os, const std::string &msg){
	os << msg << " error: " << SDL_GetError() << std::endl;
}

SDL_Texture* load_texture(const std::string &file, SDL_Renderer *ren){
	SDL_Texture *texture = nullptr;
	SDL_Surface *loadedImage = SDL_LoadBMP(file.c_str());
	if (loadedImage != nullptr){
		texture = SDL_CreateTextureFromSurface(ren, loadedImage);

		SDL_FreeSurface(loadedImage);

		if (texture == nullptr){
			logSDLError(std::cout, "CreateTextureFromSurface");
		}
	}
	else {
		logSDLError(std::cout, "LoadBMP");
	}
	return texture;
}

void render_texture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y){
	SDL_Rect destination;
	destination.x = x;
	destination.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &destination.w, &destination.h);
	SDL_RenderCopy(ren, tex, NULL, &destination);
}

//Render a sprite from the given spritesheet
void render_sprite(SDL_Texture *tex, SDL_Renderer *ren, int srcx, int srcy, int x, int y) {
	SDL_Rect source;
	//Create a source rectangle
	source.x = srcx;
	source.y = srcy;
	source.h = TILE_SIZE;
	source.w = TILE_SIZE;
	//Create the destination rectangle
	SDL_Rect destination;
	destination.x = x;
	destination.y = y;
	destination.h = TILE_SIZE;
	destination.w = TILE_SIZE;
	//Render the source to the destination
	SDL_RenderCopy(ren, tex, &source, &destination);
}

//Write out a message to the ingame user in the text output
void add_to_log(string message) {
	logoutput.push_back(message);
}

void render_game_log(SDL_Texture *tex, SDL_Renderer *ren) {
	int line = 0;
	int current_line_position = 0;
	int location;

	for (vector<string>::reverse_iterator rit = logoutput.rbegin(); rit != logoutput.rend(); ++rit) {
		if (line > LOG_LINE_COUNT) break;
		if ((*rit).size() > (uint)LOG_LINE_LENGTH) {
			//Handle multiline comment later
		} else {
			current_line_position = 0;
			for (string::iterator sit = (*rit).begin(); sit != (*rit).end(); ++sit) {
				if(isupper(*sit)) {
					location = *sit - 65;
					render_sprite(tex, ren, location * TILE_SIZE + UPPER_CHAR_START, 0, LOG_LOWER_LEFT[0] * TILE_SIZE + (current_line_position++ * TILE_SIZE), LOG_LOWER_LEFT[1] * TILE_SIZE - (line * TILE_SIZE));
				} else if (islower(*sit))  {
					location = *sit - 97;
					render_sprite(tex, ren, location * TILE_SIZE + LOWER_CHAR_START, 0, LOG_LOWER_LEFT[0] * TILE_SIZE + (current_line_position++ * TILE_SIZE), LOG_LOWER_LEFT[1] * TILE_SIZE - (line * TILE_SIZE));
				}
				//Only characters for now
			}
		}
		line++;
	}
}

//Basic euclidean distance
double distance(int y1, int x1, int y2, int x2) {
	return sqrt( abs( (int) (pow((y2 - y1), 2) + pow(x2 - x1, 2)) ) );
}

void do_fov(float x,float y, Player *player) {
	int i;
	float ox,oy;
	ox = (float)player->xlocation+0.5f;
	oy = (float)player->ylocation+0.5f;
	for(i=0;i<FOV_RADIUS;i++) {
		if ((int)ox < 0 || (int)ox > 59) continue;
		if ((int)oy < 0 || (int)oy > 59) continue;
		player->world[player->currentfloor][(int)ox][(int)oy] = gameworld[player->currentfloor][(int)ox][(int)oy];
		player->color_mesh[(int)ox][(int)oy] = 1;
		if(gameworld[player->currentfloor][(int)ox][(int)oy]=='#') return;
		ox += x;
		oy += y;
	};
}

void FOV (Player *player) {
	float x,y;
	int i;
	player->clear_color_mesh();
	for(i=0;i<360;i++) {
		x=cos((float)i*0.01745f);
		y=sin((float)i*0.01745f);
		do_fov(x,y,player);
	};

}


void render_player_world(SDL_Texture *tex, SDL_Renderer *ren, Player *player) {
	int offset;
	for (size_t i = 0; i < 60; i++) {
		for (size_t j = 0; j < 60; j++) {
			if (player->world[player->currentfloor][j][i] != '=') {
				//render bright tiles
				if (player->color_mesh[j][i] == 1) {
					offset = LIGHT_SPRITE_Y_START;
				} else {
					offset = DARK_SPRITE_Y_START;
				}
				switch (player->world[player->currentfloor][j][i]) {
					case '#':
						//Render wall
						render_sprite(tex, ren, WALL_SPRITE_OFFSET, offset, j * TILE_SIZE, i * TILE_SIZE);
						break;
					case '.':
						render_sprite(tex, ren, FLOOR_SPRITE_OFFSET, offset, j * TILE_SIZE, i * TILE_SIZE);
						break;
					case '+':
						render_sprite(tex, ren, DOOR_SPRITE_OFFSET, offset, j * TILE_SIZE, i * TILE_SIZE);
						break;
					default:
						render_sprite(tex, ren, WALL_SPRITE_OFFSET, offset, j * TILE_SIZE, i * TILE_SIZE);
						break;
				}
			}
		}
	}
}

bool try_action(int action, Player *player) {
	set<char> chars;
	int player_offset[2] = {0, 0};
	chars.insert(IMPASSIBLE_TERRAIN, IMPASSIBLE_TERRAIN + strlen(IMPASSIBLE_TERRAIN));

	switch (action) {
		case NORTH:
			player_offset[1]--;
			break;
		case SOUTH:
			player_offset[1]++;
			break;
		case EAST:
			player_offset[0]++;
			break;
		case WEST:
			player_offset[0]--;
			break;
		case NE:
			player_offset[0]++;
			player_offset[1]--;
			break;
		case NW:
			player_offset[0]--;
			player_offset[1]--;
			break;
		case SE:
			player_offset[0]++;
			player_offset[1]++;
			break;
		case SW:
			player_offset[0]--;
			player_offset[1]++;
			break;
		case STILL:
			break;
		default:
			break;
	}
	if (chars.find(gameworld[player->currentfloor][player->xlocation + player_offset[0]][player->ylocation + player_offset[1]]) != chars.end())  {
		add_to_log(string("Bump!"));
		return false;
	} else {
		player->xlocation += player_offset[0];
		player->ylocation += player_offset[1];
		return true;
	}
}

void handle_keypress(Player *player, SDL_Event event) {
	switch(event.key.keysym.sym) {
		case SDLK_LEFT:
		case SDLK_KP_4:
			try_action(WEST, player);
			break;
		case SDLK_RIGHT:
		case SDLK_KP_6:
			try_action(EAST, player);
			break;
		case SDLK_UP:
		case SDLK_KP_8:
			try_action(NORTH, player);
			break;
		case SDLK_DOWN:
		case SDLK_KP_2:
			try_action(SOUTH, player);
			break;
		case SDLK_KP_9:
			try_action(NE, player);
			break;
		case SDLK_KP_7:
			try_action(NW, player);
			break;
		case SDLK_KP_3:
			try_action(SE, player);
			break;
		case SDLK_KP_1:
			try_action(SW, player);
			break;
		case SDLK_KP_5:
			try_action(STILL, player);
			break;
		default:
		break;
	}
}


int main(int, char**){

	usleep(4000);
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	SDL_Window *window = SDL_CreateWindow("Perun", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	assertptr(window, "SDL_CreateWindow Error: ");

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assertptr(renderer, "SDL_CreateRenderer Error: ");

	SDL_Texture *background = load_texture("res/adat_layout.bmp", renderer);
	SDL_Texture *spritesheet = load_texture("res/adat_sprites.bmp", renderer);
	SDL_Event event;

	read_world();

	Player player;
	player.xlocation = 14;
	player.ylocation = 14;
	player.hasmoved = false;
	player.currentfloor = 0;
	player.initialize_player_world();
	int quit = 0;
	//Main game loop

	add_to_log(string("Hello"));
	add_to_log(string("How"));
	add_to_log(string("Are"));
	while(!quit) {
		//First clear the renderer
		SDL_RenderClear(renderer);
		//Draw the texture
		while (!SDL_PollEvent(&event)) {};
		do {
			//Handle input
			switch(event.type) {
				case SDL_QUIT:
					quit = 1;
					break;
					case SDL_KEYDOWN:
						handle_keypress(&player, event);
						break;
					default:
						break;
				}
			} while (SDL_PollEvent(&event));

		//Generate player field of view
		FOV(&player);
		render_texture(background, renderer, 0, 0);
		render_game_log(spritesheet, renderer);

		//Render world
		render_player_world(spritesheet, renderer, &player);
		//Render player character last
		render_sprite(spritesheet, renderer, (5 * TILE_SIZE), (5 * TILE_SIZE), (player.xlocation * TILE_SIZE), (player.ylocation * TILE_SIZE));
		//Update the screen
		SDL_RenderPresent(renderer);
		//Take a quick break after all that hard work
	}

	cleanup(renderer, window, background);
	SDL_Quit();
	return 0;
}
