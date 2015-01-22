#ifndef __JSONNUMBER_HH__
#define __JSONNUMBER_HH__

#include "JsonValue.hh"

#define VALID_NUMBER_CHARS "+-0123456789.eE"

class JsonNumber : public JsonValue {

 private:

 protected:

  double value_;

 public:

  static uint8_t type_;

  JsonNumber() {
    value_ = 0.0;
  }

  JsonNumber(double d) { value_ = d; }

  uint8_t getType() { return type_; }

  double getNumber() { return value_; }

  void setNumber(double num) { value_ = num; } 

  double toDouble() { return value_; }

  int toInteger() { return (int )value_; }

  bool toBoolean() { return value_ != 0.0 ? true : false; }

  const char *toString(char *buffer, uint32_t length) {
    sprintf(buffer, "%f", value_);
    return buffer;
  }

};

#endif