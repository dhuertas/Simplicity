#include "Simulation.hh"
#include "JsonReader.hh"
#include "Chrono.hh"

//------------------------------------------------------------------------------
Simulation::Simulation() : 
  domain_(NULL), 
  config_(NULL),
  fes_(NULL),
  currentEvent_(NULL) {

  fes_ = new BinaryHeap();
  fes_->setComparator(Event::compare);

  domain_ = new Domain();

  currentTime_ = Time(0);
}

//------------------------------------------------------------------------------
Simulation::~Simulation() {
  reset();
}

//------------------------------------------------------------------------------
int Simulation::configure(const char *fileName) {

  if (fileName == NULL) {
    ERROR("Missing file name");
    return -1;
  }

  JsonReader reader;
  bool success = false;

  success = reader.parse(fileName, &config_);

  if ( ! success) {
    ERROR("Unable to parse configuration file");
    return -1;
  }

  if ( ! config_->isObject()) {
    ERROR("Configuration file must be a valid JSON object");
    return -1;
  }

  //
  // Globals
  //
  JsonValue *globalConfig = (*config_)["global"];
  if (globalConfig->isNull() || ! globalConfig->isObject()) {
    ERROR("Globals must be a valid JSON object");
    return -1;
  }

  // Global.logFileName
  JsonValue *logFileName = (*globalConfig)["logFileName"];
  if (logFileName != NULL) {
    if (! logFileName->isString()) {
      ERROR("Log file name must be a string");
    } else {
      Logger::setRedirect(logFileName->toString().c_str());
    }
  } 

  // Global.timeScaleExp
  JsonValue *timeScaleExp = (*globalConfig)["timeScaleExp"];
  if ( ! timeScaleExp->isNumber()) {
    ERROR("Time scale exponent must be a number");
    return -1;
  }
  Time::setScaleExp(timeScaleExp->toInteger());

  // Gobal.timeLimit
  JsonValue *timeLimit = (*globalConfig)["timeLimit"];
  if ( ! timeLimit->isNumber()) {
    ERROR("Time limit must be a number");
    return -1;
  }
  timeLimit_.setTime(timeLimit->toDouble());

  // Global.timeStart
  JsonValue *timeStart = (*globalConfig)["timeStart"];
  if ( ! globalConfig or ! globalConfig->isNumber()) {
    INFO("Start time: 0 (default)");
    timeStart_ = Time(0);
  } else {
    timeStart_ = Time(timeStart->toDouble());
  }

  currentTime_ = timeStart_;

  //
  // Domain
  //
  JsonValue *domainConfig = (*config_)["domain"];
  if (domainConfig == NULL || domainConfig->isNull() || ! domainConfig->isObject()) {
    ERROR("Domain must be a valid JSON object");
    return -1;
  }

  JsonValue *modulesConfig = (*domainConfig)["modules"];
  if (modulesConfig == NULL || modulesConfig->isNull() || ! modulesConfig->isObject()) {
    ERROR("Missing modules in domain");
    return -1;
  }

  std::vector<JsonObjectPair>::iterator it;
  for (it = modulesConfig->begin(); it != modulesConfig->end(); ++it) {
    Module *module = createModule((*it).second->getString());
    if (module != NULL) {
      // Use the json object parameter key as module name
      // It is later used to initialize each module from the init file
      module->setName((*it).first);
    }
  }

  JsonValue *initFileName = (*domainConfig)["initFileName"];

  if ( ! initFileName->isString()) {
    ERROR("Expected initialization file name");
    return -1;
  }

  INFO("Initializing domain");
  domain_->initialize(initFileName->toString());

  //
  // Web server
  //
  JsonValue *webServerConfig = (*config_)["webserver"];
  if (webServerConfig != NULL && ! webServerConfig->isNull()) {
    INFO("Spawning web server");
    server_ = new WebServer();

    if (server_ == NULL) {
      ERROR("Unable to spawn web server");
      return -1;
    }

    server_->setSimulation(this);

    server_->start();
  }

  return 0;
}

//------------------------------------------------------------------------------
void Simulation::run() {

  Module *currentModule = NULL;

  INFO("Running ...");

  long long unsigned eventCount = 0;

  Time *now = NULL;

  Chrono chronometer;
  double elapsed = 0;

  chronometer.start();

  while (fes_->size() > 0) {

    currentEvent_ = (Event *)fes_->pop();
    currentEvent_->setScheduled(false);

    if ( ! currentEvent_) {
      ERROR("null event");
      break;
    }

    now = currentEvent_->getTime();
    currentTime_.setRaw(now->raw());

    if (*now >= timeLimit_) {
      INFO("Time limit reached");
      break;
    }

    currentModule = currentEvent_->getTo();

    if ( ! currentModule) {
      ERROR("null module");
      break;
    }

    //DEBUG("Event(%llu) time(%.012f) type(%u)", eventCount, now->dbl(), currentEvent_->getType());

    currentModule->handleEvent(currentEvent_);

    // Check for deleted modules
    if (domain_->deletedModules() > 0)
      domain_->clearDeletedModules();

    eventCount++;
  }

  elapsed = chronometer.elapsed();
  INFO("Execution time %.06f seconds; %llu events; %g events/second; avg simsec/second %.06f", 
    elapsed,
    eventCount,
    eventCount/elapsed,
    currentTime_.dbl()/elapsed);

  if (fes_->size() == 0) {
    INFO("Empty FES");
  }
}

//------------------------------------------------------------------------------
void Simulation::finalize() {

  domain_->finalize();

  reset();
}

//------------------------------------------------------------------------------
void Simulation::reset() {

  if (config_ != NULL) {
    INFO("Removing config");
    delete config_;
    config_ = NULL;
  }

  if (fes_ != NULL) {
    INFO("Removing FES");
    delete fes_;
    fes_ = NULL;
  }

  if (domain_ != NULL) {
    INFO("Removing simulation domain");
    delete domain_;
    domain_ = NULL;
  }

  if (server_ != NULL) {
    server_->stop();
    INFO("Removing web server");
    server_->join();
    delete server_;
    server_ = NULL;
  }
}

//------------------------------------------------------------------------------
Module* Simulation::getModule(int id) {

  return domain_->getModule(id);
}

//------------------------------------------------------------------------------
Module* Simulation::getModule(std::string name) {

  return domain_->getModule(name);
}

//------------------------------------------------------------------------------
Module* Simulation::createModule(std::string moduleName) {

  Module *module = CreateModule(moduleName);

  INFO("Creating %s module", moduleName.c_str());

  if ( ! module) {
    INFO("Unable to create module");
    return NULL;
  }

  if ( ! fes_) {
    INFO("Unable to set FES to module");
    delete module;
    return NULL;
  }

  module->setFes(fes_);

  if ( ! domain_) {
    INFO("Unable to add module to domain");
    delete module;
    return NULL;
  }

  domain_->addModule(module);

  return module;
}

//------------------------------------------------------------------------------
void Simulation::deleteModule(Module *module) {
  domain_->deleteModule(module);
}
