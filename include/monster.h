#ifndef MONSTER_H
#define MONSTER_H

class Monster {
public:
	Monster(int type);
	int health;
	int maxhealth;
	int tohit;
	int damage;
	int challengerating;
	int xlocation;
	int ylocation;
	int currentfloor;
};

class MonsterFactory {
public:
	Monster make_monster();
};

#endif
