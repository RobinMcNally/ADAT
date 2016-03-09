#ifndef MONSTER_H
#define MONSTER_H

class Player;

class Monster {
public:
	Monster(int type, int towerlevel);
	int health;
	int maxhealth;
	int tohit;
	int damage;
	int challengerating;
	int xlocation;
	int ylocation;
	int currentfloor;
	char displaycharacter;
};

class MonsterFactory {
public:
	Monster make_monster();
};

#endif
