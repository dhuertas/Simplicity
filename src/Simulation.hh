#ifndef __SIMULATION_HH__
#define __SIMULATION_HH__

#include "Object.hh"
#include "Event.hh"
#include "BinaryHeap.hh"
#include "ModuleFactory.hh"
#include "JsonValue.hh"
#include "Time.hh"
#include "Domain.hh"
#include "MersenneTwister.hh"

#define Sim \
  Simulation::getInstance()

class Simulation : public Object {

 private:

 protected:

  Domain *domain_;

  JsonValue *config_;

  BinaryHeap *fes_;

  Time timeStart_;

  Time timeLimit_;

  Event *currentEvent_;

  Time currentTime_;

  std::vector<MersenneTwister *> rng_;

 public:

  Simulation();

  ~Simulation();

  static Simulation *getInstance() {
    static Simulation instance;
    return &instance;
  }

  int configure(const char *fileName, const char *path);

  void run(void);

  void finalize(void);

  void reset(void);

  Module *createModule(const std::string &moduleName);

  Module *createCompound(const std::string &moduleName, JsonValue *moduleConfig);

  void deleteModule(Module *module);

  //
  // Gets and sets
  //
  Module *getModule(int id);

  Module *getModule(std::string name);

  Domain *getDomain(void) { return domain_; }

  JsonValue *getConfig(void) { return config_; }

  Event *getCurrentEvent(void) { return currentEvent_; }

  Time *getCurrentTime(void) { return &currentTime_; }

  double getDoubleRand(uint32_t rngIdx = 0);

  int64_t getIntRand(uint32_t rngIdx = 0);

};

#endif
