#ifndef __SIMULATION_HH__
#define __SIMULATION_HH__

#include "Object.hh"
#include "Event.hh"
#include "BinaryHeap.hh"
#include "ModuleFactory.hh"
#include "JsonValue.hh"
#include "Time.hh"
#include "Domain.hh"
#include "WebServer.hh"

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

  pthread_t serverThread_;

 public:

  Simulation();

  ~Simulation();

  static Simulation *getInstance() {
    static Simulation instance;
    return &instance;
  }

  int configure(const char *fileName);

  void run();

  void finalize();

  void reset();

  Module *getModule(int id);

  Module *getModule(std::string name);

  Domain *getDomain() { return domain_; }

  JsonValue *getConfig() { return config_; }

  Event *getCurrentEvent() { return currentEvent_; }

  Time *getCurrentTime() { return &currentTime_; }

  Module *createModule(std::string moduleName);

  void deleteModule(Module *module);

};

#endif