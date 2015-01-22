#ifndef __MODULEFACTORY_HH__
#define __MODULEFACTORY_HH__

#include <string>
#include <map>

#include "Object.hh"
#include "Module.hh"

typedef Module* (*CreateModuleFn)(void);

typedef std::map<std::string, CreateModuleFn> FactoryMap;

class ModuleFactory {

 private:

 protected:

  FactoryMap map_;

 public:

  static ModuleFactory *getInstance() {
    static ModuleFactory instance;
    return &instance;
  }

  bool registerModule(std::string moduleName, CreateModuleFn pFnCreate);

  Module *createModule(std::string moduleName);

};

#define RegisterModule( _name ) \
  Module* create_ ## _name() {  return new _name; } \
  static bool _name ## _creator_registered = ModuleFactory::getInstance()->registerModule(# _name, create_ ## _name);
//  static bool _name ## _creator_registered = ModuleFactory::registerModule(# _name, create_ ## _name);

#define CreateModule(name) \
  ModuleFactory::getInstance()->createModule(name);

#endif