#ifndef __BASEMODULE_HH__
#define __BASEMODULE_HH__

#include "Simplicity.hh"

class BaseModule : public Module {

 private:

 protected:

 public:

   void handleEvent(Event *ev);

   void initialize(int stage);

   void finalize();

   int initStages(void) { return 1; }

};

#endif
