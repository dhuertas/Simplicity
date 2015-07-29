#include "Random.hh"
#include "Simulation.hh"

double Random::exponential(double mean) {
  return -mean*log(Sim->getDoubleRand());
}
