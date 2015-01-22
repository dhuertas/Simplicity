#ifndef __TICTOC_HH__
#define __TICTOC_HH__

#include "Brainstorm.hh"

class Tictoc : public Module {

 private:

 protected:

  Module *tictoc_;

  Event *event_;

  Time delay_;

  Record rec_;

 public:

  Tictoc();

  void handleEvent(Event *ev);

  void initialize(int stage);

  void finalize();

};

class TicTocData : public RecordEntry {

 public:

  Time t_;

  std::string name_;

  std::string serialize() {
    std::stringstream ss;
    ss << t_.dbl() << "," << name_;
    return ss.str();
  }

};

#endif