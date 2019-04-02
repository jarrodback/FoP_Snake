/*
**A simple library of random numbers routines  
*/

#ifndef RandomUtilsH
#define RandomUtilsH

#include <stdlib.h>		//for rand and srand
#include <ctime>		//for time used in random number routines
#include <cassert>		//for assert

//Seed() - seeds the random number generator from current system time 
void seed();

//Random(max) - produces a random number in range [1..max]
//pre-condition: max > 0
int random(int max); 

#endif