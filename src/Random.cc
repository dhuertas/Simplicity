#include <cmath>

#include "Random.hh"
#include "Simulation.hh"

double Random::exponential(double mean, uint32_t rngIdx) {

  return -mean*log(Sim->getDoubleRand(rngIdx));
}

double Random::uniform(double a, double b, uint32_t rngIdx) {

  return a + Sim->getDoubleRand(rngIdx) * (b-a);
}

double Random::normal(double m, double d, uint32_t rngIdx) {

  double r = Sim->getDoubleRand();
  double s = Sim->getDoubleRand();

  return m + d * sqrt(-2.0*log(1-r))*cos(2*M_PI*s);
}
