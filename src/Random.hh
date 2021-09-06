#ifndef __RANDOM_HH__
#define __RANDOM_HH__

#include <stdint.h>

class Random {

 private:

 protected:

 public:

  static double exponential(double mean, uint32_t rngIdx = 0);

  static double uniform(double a, double b, uint32_t rngIdx = 0);

  static double normal(double mean, double dev, uint32_t rngIdx = 0);

};

#endif
