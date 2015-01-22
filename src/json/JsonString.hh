#ifndef __JSONSTRING_HH__
#define __JSONSTRING_HH__

#include <string>
#include <cstring>

#include "JsonValue.hh"

using namespace std;

class JsonString : public JsonValue {

 private:

 protected:

  string value_;

 public:

  static uint8_t type_;

  JsonString() {}

  JsonString(string str) {
    value_ = str;
  }

  JsonString(const char *str) {
    value_ = std::string(str);
  }

  uint8_t getType() { return type_; }

  string getString() { return value_; }

  const char *toString(char *buffer, uint32_t length) {
    strncpy(buffer, value_.c_str(), length);
    return buffer;
  }
};

#endif