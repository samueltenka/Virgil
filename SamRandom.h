#ifndef SAMRANDOM_H
#define SAMRANDOM_H

#include <random>
#include <time.h>

namespace SamRandom {
   void init_random() {srand(time(NULL));}
   // all return non-negative values:
   int get_random() {return rand();}
   int get_random(int mod) {return rand()%mod;}
   double get_random(double mod) {return ((double)rand())/RAND_MAX;}
}

#endif
