#include "ModuleFactory.hh"

//------------------------------------------------------------------------------
bool ModuleFactory::registerModule(const std::string &moduleName, CreateModuleFn pFnCreate) {
  map_[moduleName] = pFnCreate;
  return true;
}

//------------------------------------------------------------------------------
Module *ModuleFactory::createModule(const std::string &moduleName) {

  FactoryMap::iterator it = map_.find(moduleName);

  if (it != map_.end()) {
    return (Module *)it->second();
  }

  DEBUG("Module %s not found", moduleName.c_str());
  return NULL;
}
