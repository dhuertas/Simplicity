#include "Module.hh"

//------------------------------------------------------------------------------
Module::Module() :
  id_(0),
  fes_(NULL),
  params_(NULL),
  parent_(NULL) {

}

//------------------------------------------------------------------------------
Module::~Module() {

}

//------------------------------------------------------------------------------
void Module::scheduleEvent(Event *ev, Time *t) {

  if ( ! ev) {
    ERROR("Unable to schedule empty event");
    return;
  }

  Object *obj = (Object *)ev;
  ev->setTime(*t);

  fes_->push(obj);
  ev->setScheduled(true);
}

//------------------------------------------------------------------------------
void Module::cancelEvent(Event *ev) {

  if ( ! ev) {
    ERROR("Unable to cancel empty event");
    return;
  }

  Object *obj = (Object *)ev;
  fes_->remove(obj);
  ev->setScheduled(false);
}

//------------------------------------------------------------------------------
JsonValue* Module::getParam(std::string paramName) {

  if ( ! params_) {
    ERROR("Missing params");
    return NULL;
  }

  return (*params_)[paramName];
}

//------------------------------------------------------------------------------
void Module::setParams(JsonValue *params)  {

  if ( ! params) {
    ERROR("Missing params");
    return;
  }

  DEBUG("Module %s params: %s", name_.c_str(), params->toString().c_str());
  params_ = params;
}

//------------------------------------------------------------------------------
void Module::setParent(Module *module) {

  if (module != NULL) {
    parent_ = module;
  } else {
    ERROR("Unable to set parent module");
  }
}
