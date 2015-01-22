#include "JsonObject.hh"

uint8_t JsonObject::type_ = JsonValue::OBJECT_VALUE;

//------------------------------------------------------------------------------
JsonValue* JsonObject::find(const char *key) {
  string str(key);
  return find(str);
}

//------------------------------------------------------------------------------
JsonValue* JsonObject::find(std::string key) {

  vector<JsonObjectPair>::iterator it;

  for (it = elements_.begin(); it != elements_.end(); ++it) {
    if ((*it).first.compare(key) == 0) {
      return (*it).second;
    }
  }

  return NULL;
}

//------------------------------------------------------------------------------
JsonValue* JsonObject::operator[](const char *str) {
  return find(str);
}

//------------------------------------------------------------------------------
JsonValue* JsonValue::operator[](const char *key) {

  if (getType() != OBJECT_VALUE) {
    DEBUG("Is not an object");
    return NULL;
  }

  JsonObject *obj = (JsonObject *)this;
  return obj->find(key);
}

//------------------------------------------------------------------------------
JsonValue* JsonValue::operator[](std::string key) {

  if (getType() != OBJECT_VALUE) {
    DEBUG("Is not an object");
    return NULL;
  }

  JsonObject *obj = (JsonObject *)this;
  return obj->find(key.c_str());
}