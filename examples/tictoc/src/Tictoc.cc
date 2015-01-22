#include "Tictoc.hh"

RegisterModule(Tictoc);

Tictoc::Tictoc() :
  tictoc_(NULL) {

}

void Tictoc::initialize(int stage) {

  if (this->isName("toc")) {

    tictoc_ = Sim->getModule("tic");

  } else {

    tictoc_ = Sim->getModule("toc");

    event_ = new Event(this);

    event_->setTo(tictoc_);
    event_->setTime(Time(delay_));

    scheduleEvent(event_, event_->getTime());
  }

  rec_.setName(getName());

  delay_ = Time(getParam("delay")->toDouble());

}

void Tictoc::handleEvent(Event *ev) {

  Time nextTime = Time::add(ev->getTime(), &delay_);

  TicTocData *d = new TicTocData();
  d->t_ = nextTime;
  d->name_ = this->getName().c_str();

  rec_.add(d);

  DEBUG("%s nextTime %g", this->getName().c_str(), nextTime.dbl());
  ev->setTime(nextTime);

  // Change event recipient and destination
  Module *tmp = ev->getFrom();
  ev->setFrom(this);
  ev->setTo(tmp);

  scheduleEvent(ev, &nextTime);

}

void Tictoc::finalize() {

  if (event_ != NULL) {
    delete event_;
  }
}