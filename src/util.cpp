#include "../include/util.h"
#include <cstdlib>
#include <ctime>

using namespace std;

//Dice roller function!!
int roll_dice(int count, int sides) {
	int sum = 0;
	srand(time(NULL));

	for (int i = 0; i < count; i++) {
		sum += (rand() % sides) + 1;
	}

	return sum;
}
