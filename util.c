#include <math.h>
#include <limits.h>
#include <sys/random.h>
#include "util.h"

long double rndFloat() {
	static long long rndBuf;
	getrandom(&rndBuf, sizeof(long long), 0);
	return fabsl((long double) rndBuf / (long double) LLONG_MAX);
}
