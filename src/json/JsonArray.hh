#ifndef __JSONARRAY_HH__
#define __JSONARRAY_HH__

#include <vector>

#include "JsonValue.hh"

using namespace std;

class JsonArray : public JsonValue {

 private:

 protected:

  vector<JsonValue *> values_;

 public:

  static uint8_t type_;

  JsonArray();

  ~JsonArray();

  void push(JsonValue *val) {
    values_.push_back(val);
  };

  uint8_t getType() { return type_; }

  const char *toString(char *buffer, uint32_t length) {
    snprintf(buffer, length, "%s", "array");
    return buffer;
  }

  JsonValue* operator[](uint32_t i);
};

#endif