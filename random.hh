#ifndef RANDOM_HH
#define RANDOM_HH

#include <stdlib.h>

using namespace std;

/* Generates an integer in [min, max] */
inline int generateRandomNumber (int min, int max) {
	double rand_aux = ((double)rand() / ((double)(RAND_MAX)+(double)(1)));
	int r = (int) (rand_aux * (double)(max-min+1)) + min;
	return r;
}

/* Generates a real number in [0, 1] */
inline double generateDoubleRandomNumber() {
	return ((double)rand() / ((double)(RAND_MAX)));
}

#endif
