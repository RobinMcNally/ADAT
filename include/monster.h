#ifndef MONSTER_H
#define MONSTER_H

class Monster {
public:
	int health;
	int maxhealth;
	int tohit;
	int damage;
	int challengerating;
	int xlocation;
	int ylocation;
	int currentfloor;
};

class Goblin : public Monster {
public:
	Goblin();
};

#endif
