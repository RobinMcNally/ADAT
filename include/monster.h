#ifndef MONSTER_H
#define MONSTER_H

class Player;

class Monster {
public:
	Monster(int type, int towerlevel);
	string name;
	int health;
	int maxhealth;
	int tohit;
	int damage;
	int defense;
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
