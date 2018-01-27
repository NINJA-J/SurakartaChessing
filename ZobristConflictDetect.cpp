#include <iostream>
#include <random>
#include <time.h>
#include <map>

using namespace std;

int zobrist[6][6][3];
map<int, int> conflicts;

int main() {
	int times = 10000;
	srand((int)time(NULL));
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			for (int k = 0; k < 3; k++) {
				zobrist[i][j][k] = rand();
			}
		}
	}
	for (int i = 0; i < times; i++) {
		int zobr = 0;
		for (int j = 0; j < 6; j++) {
			for (int k = 0; k < 6; k++) {
				zobr ^= zobrist[j][k][rand() % 3];
			}
		}
	}
}