#ifndef __JSONVALUE_HH__
#define __JSONVALUE_HH__

#include <string>
#include <vector>

#include "Object.hh"

class JsonValue;

using namespace std;

typedef pair<string, JsonValue *> JsonObjectPair;

class JsonValue : public Object {

 public:

  enum {
    NULL_VALUE = 0,
    OBJECT_VALUE, 
    ARRAY_VALUE,
    STRING_VALUE,
    NUMBER_VALUE,
    BOOLEAN_VALUE
};

 private:

 protected:

 public:

  vector<JsonObjectPair> elements_;

  static uint8_t nullType_;

  bool isNull() { return (getType() == NULL_VALUE); }

  bool isObject() { return (getType() == OBJECT_VALUE); }

  bool isArray() { return (getType() == ARRAY_VALUE); }

  bool isString() { return (getType() == STRING_VALUE); }

  bool isNumber() { return (getType() == NUMBER_VALUE); }

  bool isBoolean() { return (getType() == BOOLEAN_VALUE); }

  virtual ~JsonValue() {}

  virtual uint8_t getType() { return nullType_; }

  virtual string getString() { return std::string("null"); }

  virtual string toString();

  virtual const char *toString(char *buffer, uint32_t length);

  virtual double toDouble();

  virtual int toInteger();

  virtual bool toBoolean();

  virtual vector<JsonObjectPair>::iterator begin() { return elements_.begin(); }

  virtual vector<JsonObjectPair>::iterator end() { return  elements_.end(); }

  JsonValue* getValue(const char *key);

  // Operator[] overload to access JSON Objects
  JsonValue* operator[](const char *key);

  JsonValue* operator[](string key);

  // Operator[] overload to access JSON Arrays
  JsonValue* operator[](uint32_t i);

};

#endif