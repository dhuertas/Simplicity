#include "JsonArray.hh"

uint8_t JsonArray::type_ = JsonValue::ARRAY_VALUE;

//------------------------------------------------------------------------------
JsonArray::JsonArray() {

}

//------------------------------------------------------------------------------
JsonArray::~JsonArray() {

  vector<JsonValue *>::iterator it;
  for (it = values_.begin(); it != values_.end(); ++it) {
    delete *it;
  }
}

//------------------------------------------------------------------------------
JsonValue* JsonArray::operator[](uint32_t i) {

  if (i >= values_.size())
    return NULL;

  return values_[i];
}

//------------------------------------------------------------------------------
JsonValue* JsonValue::operator[](uint32_t i) {

  if (getType() != ARRAY_VALUE) {
    DEBUG("Is not an array");
    return NULL;
  }

  JsonArray *arr = (JsonArray *)this;
  return (*arr)[i];
}