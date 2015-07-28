#ifndef __DUDE_HH__
#define __DUDE_HH__

#include "Simplicity.hh"

class Dude : public Module {

 private:

 protected:

  Time lifespan_;

  Event *die_;

 public:

  Dude();

  void initialize(int stage);

  void finalize();

  void handleEvent(Event *ev);

  int initStages() { return 2; }

  void dieAfter(double n) { lifespan_ = Time(n); };
};

#endif
