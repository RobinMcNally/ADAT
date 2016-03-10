#include "../include/includes.h"

using namespace std;

extern vector<string> logoutput;

//Dice roller function!!
int roll_dice(int count, int sides) {
	int sum = 0;
	// srand(time(NULL));

	for (int i = 0; i < count; i++) {
		sum += (rand() % sides) + 1;
	}

	return sum;
}

int sign(int tocheck) {
	if (tocheck < 0) return -1;
	if (tocheck == 0) return 0;
	if (tocheck > 0) return 1;
	return 0;
}

//Write out a message to the ingame user in the text output
void add_to_log(string message) {
	logoutput.push_back(message);
}
