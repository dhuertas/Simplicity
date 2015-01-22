#ifndef __DOMAIN_HH__
#define __DOMAIN_HH__

#include <vector>
#include <string>

#include "Object.hh"
#include "Module.hh"
#include "JsonValue.hh"

class Domain : public Object {

 private:

 protected:

  JsonValue *init_;

  std::vector<Module *> modules_;

  std::vector<Module *> deletedModules_;

  int maxInitStages_;

  unsigned int nextModuleId_;

 public:

  Domain();

  ~Domain();

  int initialize(std::string initFileName);

  void finalize();

  void addModule(Module *module);

  Module* getModule(unsigned int id);

  Module* getModule(std::string name);

  void deleteModule(Module *module);

  int deletedModules() { return deletedModules_.size(); }

  void clearDeletedModules();
};

#endif