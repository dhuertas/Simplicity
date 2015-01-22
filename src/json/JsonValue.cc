#include "JsonValue.hh"

uint8_t JsonValue::nullType_ = JsonValue::NULL_VALUE;

//------------------------------------------------------------------------------
string JsonValue::toString() {

  if (this->isString()) {

    return this->getString();

  } else if (this->isNumber() || this->isBoolean()) {

    char buffer[2048];
    this->toString(buffer, 2048);
    return std::string(buffer);

  } else if (this->isObject()) {

    return std::string("[Object]");

  } else if (this->isArray()) {

    return std::string("[Array]");

  }

  return std::string("null");
}

//------------------------------------------------------------------------------
const char *JsonValue::toString(char *buffer, uint32_t length) {

  snprintf(buffer, length, "null");
  return buffer;
}

//------------------------------------------------------------------------------
double JsonValue::toDouble() {

  if ( ! this->isNumber() && ! this->isBoolean()) {
    ERROR("Unable to get double from %s", this->toString().c_str());
    return 0.0;
  }

  return this->toDouble();
}

//------------------------------------------------------------------------------
int JsonValue::toInteger() {

  if ( ! this->isNumber() && ! this->isBoolean()) {
    ERROR("Unable to get double from %s", this->toString().c_str());
    return 0.0;
  }

  return this->toInteger();
}

//------------------------------------------------------------------------------
bool JsonValue::toBoolean() {

  if ( ! this->isNumber() && ! this->isBoolean()) {
    ERROR("Unable to get boolean from %s", this->toString().c_str());
    return false;
  }

  return this->toBoolean();
}