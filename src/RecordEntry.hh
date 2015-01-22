#ifndef __RECORDENTRY_HH__
#define __RECORDENTRY_HH__

#include <string>

class RecordEntry {

 public:

  virtual ~RecordEntry() { }

  virtual std::string serialize() {
    return std::string("null");
  }

};

#endif