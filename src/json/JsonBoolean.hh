#ifndef __JSONBOOLEAN_HH__
#define __JSONBOOLEAN_HH__

#include "JsonValue.hh"

#define VALID_TRUE  "true"
#define VALID_FALSE "false"

class JsonBoolean : public JsonValue {

 private:

 protected:

  bool value_;

 public:

  static uint8_t type_;

  JsonBoolean() { value_ = false; }

  JsonBoolean(bool b) { value_ = b; }

  ~JsonBoolean() {}

  uint8_t getType() { return type_; }

  bool getValue() { return value_; }

  double toDouble() { return value_ ? 1.0 : 0.0; }

  int toInteger() { return value_ ? 1 : 0; }

  bool toBoolean() { return value_; }

  const char *toString(char *buffer, uint32_t length) {
    snprintf(buffer, length, "%s", value_ ? VALID_TRUE : VALID_FALSE);
    return buffer;
  }

};

#endif