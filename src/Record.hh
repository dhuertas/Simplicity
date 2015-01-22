#ifndef __RECORD_HH__
#define __RECORD_HH__

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "Object.hh"
#include "RecordEntry.hh"

class Record : public Object {

 private:

 protected:

  static long unsigned count_;

  long unsigned id_;

  std::string name_;

  std::vector<RecordEntry *> entries_;

  std::fstream fs_;

 public:

  Record();

  Record(std::string name);

  ~Record();

  void add(RecordEntry *entry);

  std::string getName() { return name_; }

  void setName(std::string name) { name_ = name; };

};

#endif