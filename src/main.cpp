#include <iostream>
#include <string>
#include <vector>
#include <fstream>
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

extern int TOWER_HEIGHT;

vector<string> logoutput;

//Notation for this is [z][x][y]
char gameworld[TOWER_HEIGHT][60][60];

class Player {
public:
	bool hasmoved;
	int xlocation;
	int ylocation;
	int currentfloor;
	char playerWorld[TOWER_HEIGHT][60][60];
};

void assertptr(void *val, std::string ErrorText) {
	if (val == nullptr) {
		cout << ErrorText << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
}

void copyLevel(ifstream *levelfile, int currentfloor) {
	string line;

	for (int i = 0; i < SCREEN_RADIUS * 2; i++) {
		getline (*levelfile, line);
		for (size_t j = 0; j < line.size(); j++) {
			gameworld[currentfloor][j][i] = line[j];
		}
	}
}

void skipLevel(ifstream *levelfile) {
	string line;

	for (int i = 0; i < SCREEN_RADIUS * 2; i++) {
		getline (*levelfile, line);
	}
}

void skipWhitespace(ifstream *levelfile) {
	string line = "";
	while (line == "") {
		getline(levelfile, line);
		if (line[0] == "*") {
			line = "";
		}
	}
}

void readworld() {
	string line;
	int floorcount;
	int randomfloor = 0;
	int parsedcount = 0;
	int currentfloor = 0;

	ifstream levelfile;
	levelfile.open ("../res/levels.adatdun");
	if (levelfile.is_open()) {
		line = skipWhitespace(levelfile);

		//Super jank to the rescue
		floorcount = stoi(line);
		srand(time(NULL));
		for (int i = 0; i < TOWER_HEIGHT; i++) {
			randomfloor = rand() % floorcount;
			while (parsedcount < floorcount) {
				//Scan through blank lines (or ones with comments) till we get one with something in it
				line = skipWhitespace(levelfile);
				for (size_t i = 0; i < line.size(); i++) {
					if (!isdigit(line[0])) {
						cout << "Improper syntax in levels file\n" << line;
					}
				}
				if (stoi(line) == randomfloor) {
					copyLevel(&levelfile, currentfloor);
				} else {
					skipLevel(&levelfile);
				}
			}
			levelfile.clear();
			levelfile.seekg(0, ios::beg);
			getline(levelfile, line);
		}
	}
}

void logSDLError(std::ostream &os, const std::string &msg){
	os << msg << " error: " << SDL_GetError() << std::endl;
}

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren){

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

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y){
	SDL_Rect destination;
	destination.x = x;
	destination.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &destination.w, &destination.h);
	SDL_RenderCopy(ren, tex, NULL, &destination);
}

void renderSprite(SDL_Texture *tex, SDL_Renderer *ren, int srcx, int srcy, int x, int y) {

	SDL_Rect source;
	source.x = srcx;
	source.y = srcy;
	source.h = TILE_SIZE;
	source.w = TILE_SIZE;

	SDL_Rect destination;
	destination.x = x;
	destination.y = y;
	destination.h = TILE_SIZE;
	destination.w = TILE_SIZE;

	SDL_RenderCopy(ren, tex, &source, &destination);
}

//Write out a message to the ingame user in the text output
void write_player_message(string message) {
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
					renderSprite(tex, ren, location * TILE_SIZE + UPPER_CHAR_START, 0, LOG_LOWER_LEFT[0] * TILE_SIZE + (current_line_position++ * TILE_SIZE), LOG_LOWER_LEFT[1] * TILE_SIZE - (line * TILE_SIZE));
				} else if (islower(*sit))  {
					location = *sit - 97;
					renderSprite(tex, ren, location * TILE_SIZE + LOWER_CHAR_START, 0, LOG_LOWER_LEFT[0] * TILE_SIZE + (current_line_position++ * TILE_SIZE), LOG_LOWER_LEFT[1] * TILE_SIZE - (line * TILE_SIZE));
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

// void FOV()
// {
//   float x,y;
//   int i;
//   CLEAR_MAP_TO_NOT_VISIBLE();//Initially set all tiles to not visible.
//   for(i=0;i<360;i++)
//   {
//     x=cos((float)i*0.01745f);
//     y=sin((float)i*0.01745f);
//     DoFov(x,y);
//   };
// };
//
// void DoFov(float x,float y)
// {
//
// };
void FOV () {
	float x,y;
	int i;
	CLEAR_MAP_TO_NOT_VISIBLE();//Initially set all tiles to not visible.
	for(i=0;i<360;i++) {
		x=cos((float)i*0.01745f);
		y=sin((float)i*0.01745f);
		DoFov(x,y);
	};

}

void DoFov(float x,float y) {
	int i;
	float ox,oy;
	ox = (float)PLAYERX+0.5f;
	oy = (float)PLAYERY+0.5f;
	for(i=0;i<FOV_RADIUS;i++) {
		playerWorld[][(int)ox][(int)oy]=VISIBLE;//Set the tile to visible.
		if(MAP[(int)ox][(int)oy]==BLOCK) return;
		ox+=x;
		oy+=y;
	};
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

	SDL_Texture *background = loadTexture("res/adat_layout.bmp", renderer);
	SDL_Texture *spritesheet = loadTexture("res/adat_sprites.bmp", renderer);
	SDL_Event event;

	readworld();

	Player player;
	player.xlocation = 14;
	player.ylocation = 14;
	player.hasmoved = false;
	int quit = 0;
	//Main game loop

	write_player_message(string("Hello"));
	write_player_message(string("How"));
	write_player_message(string("Are"));
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
					switch(event.key.keysym.sym) {
						case SDLK_LEFT:
						case SDLK_KP_4:
							player.xlocation--;
							break;
						case SDLK_RIGHT:
						case SDLK_KP_6:
							player.xlocation++;
							break;
						case SDLK_UP:
						case SDLK_KP_8:
							player.ylocation--;
							break;
						case SDLK_DOWN:
						case SDLK_KP_2:
							player.ylocation++;
							break;
						default:
						break;
					}
					break;
						default:
					break;
				}
			} while (SDL_PollEvent(&event));
		renderTexture(background, renderer, 0, 0);
		renderSprite(spritesheet, renderer, (5 * TILE_SIZE), (5 * TILE_SIZE), (player.xlocation * TILE_SIZE), (player.ylocation * TILE_SIZE));
		render_game_log(spritesheet, renderer);
		//Update the screen
		SDL_RenderPresent(renderer);
		//Take a quick break after all that hard work
	}

	cleanup(renderer, window, background);
	SDL_Quit();
	return 0;
}
