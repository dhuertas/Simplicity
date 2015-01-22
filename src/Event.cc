#include "Event.hh"

//------------------------------------------------------------------------------
Event::Event() :
  type_(0),
  owner_(NULL),
  from_(NULL),
  to_(NULL),
  scheduled_(false) {

  et_.setTime(0);
}

//------------------------------------------------------------------------------
Event::Event(Module *owner, unsigned int type) : 
  type_(type),
  owner_(owner),
  from_(owner),
  to_(NULL),
  scheduled_(false) {

}

//------------------------------------------------------------------------------
Event::Event(double d) :
  owner_(NULL),
  from_(NULL),
  to_(NULL),
  scheduled_(false) {

  et_.setTime(d);
}

//------------------------------------------------------------------------------
Event::~Event() {

  // Modules should be deleted in Domain destructor
  from_ = NULL;
  to_ = NULL;
}