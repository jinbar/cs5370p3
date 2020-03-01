 
#include "rand.h"

int temp = 1;

int xv6_rand(void) {
	unsigned int x = temp;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	temp = x;
	return x;
}

void xv6_srand(unsigned int seed) {
    temp = seed;
}