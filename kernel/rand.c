#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "spinlock.h"
#include "rand.h"

int temp;

int xv6_rand(void) {
    if (temp == NULL) {
        temp = 1;
    }
	unsigned int x = temp;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return x;
}

void xv6_srand(unsigned int seed) {
    temp = seed;
}