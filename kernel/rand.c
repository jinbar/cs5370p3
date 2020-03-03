 
#include "rand.h"

int temp = 1;

int xv6_rand(void) {
	uint x = temp;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	temp = x;
	return x % XV6_RAND_MAX;
}

void xv6_srand(unsigned int seed) {
    temp = seed;
}