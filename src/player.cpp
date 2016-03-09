#include "../include/includes.h"

// Initialize the player world to the default nonrendered character ('=')
void Player::initialize_player_world() {
	for (size_t i = 0; i < TOWER_HEIGHT; i++) {
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
