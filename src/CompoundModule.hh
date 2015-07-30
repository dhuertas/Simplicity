#ifndef __COMPOUNDMODULE_HH__
#define __COMPOUNDMODULE_HH__

#include "Module.hh"

class CompoundModule : public Module {

 private:

 protected:

  std::vector<Module *> submodules_;

 public:

   void handleEvent(Event *ev);

   void initialize(int stage);

   void finalize(void);

   void addChildModule(Module *module);

};

#endif
