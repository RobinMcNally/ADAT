#include "../include/includes.h"

Monster::Monster(int type, int towerlevel) {
	srand(time(NULL));

	maxhealth = roll_dice(1, 6);
	health = maxhealth;
	damage = roll_dice(1, 3);
	tohit = roll_dice(1, 3);
	challengerating = 1;
	displaycharacter = 'g';
	currentfloor = towerlevel;
}

Monster MonsterFactory::make_monster() {
	srand(time(NULL));

	monsters chosen = static_cast<monsters>(rand() % MONSTER_TYPE_COUNT);
	Monster monster(chosen, 0);
	return monster;
}
