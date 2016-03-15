#include "../include/includes.h"

Monster::Monster(int type, int towerlevel) {
	srand(time(NULL));
	maxhealth = roll_dice(1, 6);
	health = maxhealth;
	damage = roll_dice(1, 3);
	tohit = roll_dice(1, 3);
	defense = roll_dice(1, 3);
	challengerating = 1;
	name = "goblin";
	displaycharacter = 'g';
	currentfloor = towerlevel;
}

void Monster::attack(Player *target) {
	int playerroll = roll_dice(1, 20) + target->DEX;
	int monsterroll = roll_dice(1, 20) + tohit;

	if (monsterroll >= playerroll) {
		target->health -= damage;
	}
}

Monster MonsterFactory::make_monster() {
	srand(time(NULL));

	monsters chosen = static_cast<monsters>(rand() % MONSTER_TYPE_COUNT);
	Monster monster(chosen, 0);
	return monster;
}
