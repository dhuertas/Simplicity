#ifndef __CREATOR_HH__
#define __CREATOR_HH__

#include <vector>

#include "Brainstorm.hh"
#include "Dude.hh"

class Creator : public Module {

 private:

 protected:

  std::vector<Module *> dudes_;

 public:

  Creator();

  void initialize(int stage = 0);

  void finalize();

  void handleEvent(Event *ev);

  int initStages() { return 1; }

};

#endif