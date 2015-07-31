#include "CompoundModule.hh"

// -----------------------------------------------------------------------------
CompoundModule::CompoundModule() {

}

// -----------------------------------------------------------------------------
void CompoundModule::handleEvent(Event *ev) {

}

// -----------------------------------------------------------------------------
void CompoundModule::initialize(int stage) {
  // Initialize submodules
  std::vector<Module *>::iterator it;

  for (it = submodules_.begin(); it != submodules_.end(); ++it) {
    (*it)->initialize(stage);
  }
}

// -----------------------------------------------------------------------------
void CompoundModule::finalize(void) {
  // Call finalize to each module in submodules_
  std::vector<Module *>::iterator it;

  for (it = submodules_.begin(); it != submodules_.end(); ++it) {
    (*it)->finalize();
  }
}

// -----------------------------------------------------------------------------
void CompoundModule::addChildModule(Module *module) {

  if (module != NULL) {
    submodules_.push_back(module);
  } else {
    ERROR("Unable to add module");
  }
}
