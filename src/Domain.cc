#include "Domain.hh"
#include "JsonReader.hh"

//------------------------------------------------------------------------------
Domain::Domain() :
  init_(NULL),
  maxInitStages_(0),
  nextModuleId_(0) {

}

//------------------------------------------------------------------------------
Domain::~Domain() {

}

//------------------------------------------------------------------------------
int Domain::initialize(std::string initFileName) {

  JsonReader reader;

  if ( not reader.parse(initFileName.c_str(), &init_)) {
      ERROR("Failed parsing init config");
      return -1;
  }

  if (init_ == NULL) {
    ERROR("Unable parse initialization file %s", initFileName.c_str());
    return -1;
  }

  DEBUG("init: %s", init_->toString().c_str());

  for (int stage = 1; stage <= maxInitStages_; stage++) {
    DEBUG("Initializing %lu modules, stage %d", modules_.size(), stage);
    // Iterate each module and initialize it
    for (uint32_t i = 0, len = modules_.size(); i < len; i++) {
      Module *currentModule = modules_[i];
      // Set module parameters
      if (currentModule != NULL) {

        JsonValue *params = (*init_)[currentModule->getName()];
        currentModule->setParams(params);

        // If currentModule belongs to a compound module, skip it. It will get
        // initialized by its parent module.
        if (currentModule->getParent() != NULL)
          continue;

        // Otherwise, initialize it
        currentModule->initialize(stage);
      }
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
void Domain::finalize() {

  if (init_ != NULL) {
    delete init_;
  }

  std::vector<Module *>::iterator it;
  for (it = modules_.begin(); it != modules_.end(); ++it) {
    if (*it != NULL) {
      // Check for NULL since modules can be removed dynamically
      DEBUG("Removing module from domain");
      delete (*it);
    }
  }
}

//------------------------------------------------------------------------------
void Domain::addModule(Module *module) {

  if (module != NULL) {

    modules_.push_back(module);

    if (module->initStages() > maxInitStages_) {
      DEBUG("Initialization stages: %d -> %d",
        maxInitStages_,
        module->initStages());
      maxInitStages_ = module->initStages();
    }

    module->setId(nextModuleId_);
    nextModuleId_++;
  }
}

//------------------------------------------------------------------------------
Module* Domain::getModule(unsigned int id) {

  return id < modules_.size() ? modules_[id] : NULL;
}

//------------------------------------------------------------------------------
Module* Domain::getModule(std::string name) {

  std::vector<Module *>::iterator it;
  for (it = modules_.begin(); it != modules_.end(); ++it) {
    if ( (*it)->isName(name) ) {
      // TODO cache module for future searches
      return (*it);
    }
  }

  return NULL;
}

//------------------------------------------------------------------------------
void Domain::deleteModule(Module *module) {
  deletedModules_.push_back(module);
  // call finalize
  module->finalize();
}

//------------------------------------------------------------------------------
void Domain::clearDeletedModules() {

  std::vector<Module *>::iterator it;
  for (it = deletedModules_.begin(); it != deletedModules_.end(); ++it) {
    DEBUG("Removing module from domain");
    modules_[(*it)->getId()] = NULL;
    delete *it;
  }

  deletedModules_.clear();
}
