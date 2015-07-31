#include "Simulation.hh"
#include "JsonReader.hh"
#include "Chrono.hh"
#include "CompoundModule.hh"

//------------------------------------------------------------------------------
Simulation::Simulation() :
  domain_(NULL),
  config_(NULL),
  fes_(NULL),
  currentEvent_(NULL),
  rng_(NULL) {

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
int Simulation::configure(const char *fileName, const char *path) {

  if (fileName == NULL) {
    ERROR("Missing file name");
    return -1;
  }

  if (path == NULL) {
    ERROR("Missing path");
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
  JsonObject *globalConfig = (JsonObject *)config_->getValue("global");
  if (globalConfig == NULL or ! globalConfig->isObject()) {
    ERROR("Global must be a valid JSON object");
    return -1;
  }

  // Set Global.path so it can be accessed later
  globalConfig->push("path", new JsonString(path));

  // Global.logFileName
  JsonValue *logFileName = globalConfig->getValue("logFileName");
  if (logFileName != NULL) {
    if (! logFileName->isString()) {
      ERROR("Log file name must be a string");
    } else {
      Logger::setRedirect(logFileName->toString().c_str());
    }
  }

  // Global.timeScaleExp
  JsonValue *timeScaleExp = globalConfig->getValue("timeScaleExp");
  if (timeScaleExp == NULL or ! timeScaleExp->isNumber()) {
    ERROR("Time scale exponent must be a number");
    return -1;
  }
  Time::setScaleExp(timeScaleExp->toInteger());

  // Gobal.timeLimit
  JsonValue *timeLimit = globalConfig->getValue("timeLimit");
  if (timeLimit == NULL or ! timeLimit->isNumber()) {
    ERROR("Time limit must be a number");
    return -1;
  }
  timeLimit_.setTime(timeLimit->toDouble());

  // Global.timeStart
  JsonValue *timeStart = globalConfig->getValue("timeStart");
  if (timeStart == NULL or ! timeStart->isNumber()) {
    INFO("Start time: 0 (default)");
    timeStart_ = Time(0);
  } else {
    timeStart_ = Time(timeStart->toDouble());
  }

  currentTime_ = timeStart_;

  // Global.mtSeed
  JsonValue *mtSeed = globalConfig->getValue("mtSeed");
  if (mtSeed == NULL or ! mtSeed->isNumber()) {
    INFO("Missing Mersenne Twister RNG seed (default 0)");
    rng_ = new MersenneTwister(0);
  } else {
    INFO("Mersenne Twister RNG seed: %u", mtSeed->toInteger());
    rng_ = new MersenneTwister(mtSeed->toInteger());
  }

  //
  // Domain
  //
  JsonObject *domainConfig = (JsonObject *)config_->getValue("domain");
  if (domainConfig == NULL or domainConfig->isNull() or ! domainConfig->isObject()) {
    ERROR("Domain must be a valid JSON object");
    return -1;
  }

  JsonValue *modulesConfig = domainConfig->getValue("modules");
  if (modulesConfig == NULL or modulesConfig->isNull() or ! modulesConfig->isObject()) {
    ERROR("Missing modules in domain");
    return -1;
  }

  std::vector<JsonObjectPair>::iterator it;

  for (it = modulesConfig->begin(); it != modulesConfig->end(); ++it) {
    Module *module = NULL;
    // If second is an object, check for compound modules
    if ((*it).second->isObject()) {
      module = createCompound((*it).first, (*it).second);
    } else {
      // Otherwise, check for a string for a base module
      module = createModule((*it).second->getString());
    }
    if (module != NULL) {
      // Use the json object parameter key as module name
      // It is later used to initialize each module from the init file
      module->setName((*it).first);
    }
  }

  JsonValue *initFileName = domainConfig->getValue("initFileName");

  if ( ! initFileName->isString()) {
    ERROR("Expected initialization file name");
    return -1;
  }

  INFO("Initializing domain");
  domain_->initialize(initFileName->toString());

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
  INFO("Execution time %.06f seconds; %llu events; %g events/second; %.06f simsec/second ",
    elapsed,
    eventCount,
    elapsed > 0 ? eventCount/elapsed : 0,
    elapsed > 0 ? currentTime_.dbl()/elapsed : 0);

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

  if (rng_ != NULL) {
    INFO("Removing RNG");
    delete rng_;
    rng_ = NULL;
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
Module *Simulation::createCompound(std::string moduleName, JsonValue *moduleConfig) {

  if (moduleConfig == NULL) {
    ERROR("Missing compound module config");
    return NULL;
  }

  CompoundModule *res = NULL;

  char buffer[2048];
  memset(buffer, 0, sizeof(buffer));

  INFO("Creating compound module: %s", moduleName.c_str());

  JsonObject *config = (JsonObject *)moduleConfig;

  printf("%s", config->toString(buffer, sizeof(buffer)));

  if (moduleConfig->getValue("submodules") != NULL) {

    res = new CompoundModule();

    JsonValue *submodulesConfig = moduleConfig->getValue("submodules");
    std::vector<JsonObjectPair>::iterator it;

    for (it = submodulesConfig->begin(); it != submodulesConfig->end(); ++it) {

      Module *module = NULL;

      if ((*it).second->isObject()) {
        // If second is an object, check for a compound modules
        module = createCompound((*it).first, (*it).second);
      } else {
        // Otherwise, check for a string for a base module
        module = createModule((*it).second->getString());
      }

      if (module != NULL) {
        // Use the json object parameter key as module name
        // It is later used to initialize each module from the init file
        module->setName((*it).first);
        res->addChildModule(module);
        module->setParent(res);
      }
    }

    res->setFes(fes_);

    if ( ! domain_) {
      INFO("Unable to add compound module to domain");
      delete res;
      return NULL;
    }

    domain_->addModule(res);
  }

  return res;
}

//------------------------------------------------------------------------------
void Simulation::deleteModule(Module *module) {
  domain_->deleteModule(module);
}

//------------------------------------------------------------------------------
double Simulation::getDoubleRand() {

  if (rng_ == NULL) {
    ERROR("Missing RNG");
    return 0.0;
  }

  return rng_->genrand64_real1();
}

//------------------------------------------------------------------------------
long long unsigned Simulation::getIntRand() {

  if (rng_ == NULL) {
    ERROR("Missing RNG");
    return 0;
  }

  return (long long)rng_->genrand64_int64();
}
