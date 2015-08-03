#include <cmath>

#include "Random.hh"
#include "Simulation.hh"

double Random::exponential(double mean, uint32_t rngIdx) {
  return -mean*log(Sim->getDoubleRand(rngIdx));
}
