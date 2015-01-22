#include "Creator.hh"

RegisterModule(Creator);

Creator::Creator() { }

void Creator::initialize(int stage) {

  if (stage == 1) {

    int howMany = getParam("howMany")->toInteger();

    INFO("howMany %d", howMany);

    for (int i = 0; i < howMany; i++) {
      Dude *dude = (Dude *)Sim->createModule("Dude");
      dudes_.push_back(dude);
      dude->dieAfter(100);
      dude->setName("dude");
    }
  }

}

void Creator::finalize() {

}

void Creator::handleEvent(Event *ev) {

}