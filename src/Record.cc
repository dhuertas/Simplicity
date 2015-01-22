#include "Record.hh"

long unsigned Record::count_ = 0;
// -----------------------------------------------------------------------------
Record::Record() {

  id_ = count_;
  count_++;
}

// -----------------------------------------------------------------------------
Record::Record(std::string name) : 
  name_(name) {

  id_ = count_;
}

// -----------------------------------------------------------------------------
Record::~Record() {

  std::stringstream ss;

  ss << name_ << "_" << id_ <<".rcd";

  // 1. Open record file
  fs_.open(ss.str().c_str(), std::fstream::out);

  // 2. Serialize entries
  std::vector<RecordEntry *>::iterator it;
  for (it = entries_.begin(); it != entries_.end(); ++it) {
    RecordEntry *entry = (*it);
    fs_ << entry->serialize() << '\n';
    delete entry;
  }

  // 3. Close record
  fs_.close();
}

// -----------------------------------------------------------------------------
void Record::add(RecordEntry *entry) {
  if (entry != NULL) {
    entries_.push_back(entry);
  }
}