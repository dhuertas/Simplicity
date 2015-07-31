#ifndef __MODULE_HH__
#define __MODULE_HH__

#include <string>
#include <vector>

#include "Object.hh"
#include "BinaryHeap.hh"
#include "JsonValue.hh"
#include "Event.hh"

class Module : public Object {

 private:

 protected:

  unsigned int id_;

  BinaryHeap *fes_;

  std::string name_;

  JsonValue *params_;

  Module *parent_;

 public:

  Module();

  virtual ~Module();

  virtual void handleEvent(Event *ev) = 0;

  virtual void initialize(int stage) = 0;

  virtual void finalize(void) = 0;

  virtual int initStages() {
    // Defaults to a single initialization stage
    return 1;
  }

  void scheduleEvent(Event *ev, Time *t);

  void cancelEvent(Event *ev);

  unsigned int getId() { return id_; }

  BinaryHeap* getFes(void) { return fes_; }

  std::string getName(void) { return name_; }

  JsonValue* getParam(std::string paramName);

  Module* getParent(void) { return parent_; }

  void setId(unsigned int id) { id_ = id; }

  void setFes(BinaryHeap *bh) { fes_ = bh; }

  void setName(std::string name) { name_ = name; }

  void setParams(JsonValue* params) { params_ = params; }

  void setParent(Module *module);

  bool isName(std::string name) { return name_.compare(name) == 0; }

};

#endif
