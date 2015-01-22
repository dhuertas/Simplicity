#include "Dude.hh"

RegisterModule(Dude);

Dude::Dude() : 
  die_(NULL) {

}

void Dude::initialize(int stage) {

  if (stage == 2) {

    die_ = new Event(this);

    die_->setTo(this);
    die_->setTime(lifespan_);

    scheduleEvent(die_, &lifespan_);
  }

}

void Dude::finalize() {

}

void Dude::handleEvent(Event *ev) {

  // Received die_ event
  DEBUG("Dude must die");
  Sim->deleteModule(this);
}