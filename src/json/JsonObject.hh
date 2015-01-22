#ifndef __JSONOBJECT_HH__
#define __JSONOBJECT_HH__

#include <utility>
#include <string>
#include <vector>

#include "JsonValue.hh"

using namespace std;

class JsonObject : public JsonValue {

 public:

 private:

 protected:

 public:

  static uint8_t type_;

  JsonObject() {

  }

  ~JsonObject() {

  	vector<JsonObjectPair>::iterator it;
    for (it = elements_.begin(); it != elements_.end(); ++it) {
      delete (*it).second;
    }
  }

  void push(string key, JsonValue *value) {
  	elements_.push_back(JsonObjectPair(key, value));
  }

  void push(JsonObjectPair elem) {
  	elements_.push_back(elem);
  }

  JsonValue* find(const char *key);

  JsonValue* find(string key);

  uint8_t getType() { return type_; }

  vector<JsonObjectPair> *getElements() { return &elements_; }

  const char *toString(char *buffer, uint32_t length) {
    // TODO complete this
    return buffer;
  }

  JsonValue* operator[](const char *key);

  JsonValue* operator[](std::string key);

};

#endif