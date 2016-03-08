#include "../include/monster.h"
#include "../include/util.h"


Goblin::Goblin() {
	srand(time(NULL));

	maxhealth = roll_dice(1, 6);
	health = maxhealth;
	damage = roll_dice(1, 3);
	tohit = roll_dice(1, 3);
	challengerating = 1;
}
