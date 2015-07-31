#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "JsonReader.hh"

JsonReader::JsonReader() :
  current_(0),
  length_(0),
  buffer_(NULL) {

}

//------------------------------------------------------------------------------
JsonReader::~JsonReader() {

  reset();
}

//------------------------------------------------------------------------------
void JsonReader::reset() {

  if (buffer_ != NULL) {
    delete[] buffer_;
  }

  current_ = 0;
  length_ = 0;
}

//------------------------------------------------------------------------------
bool JsonReader::parse(const char *fileName, JsonValue **root) {

  if ( ! fileName) {
    ERROR("Missing file name");
    return false;
  }

  ifstream file(fileName, ifstream::in);

  if ( ! file.is_open()) {
    ERROR("Unable to open file: %s", fileName);
    return false;
  }

  file.seekg(0, file.end);
  length_ = file.tellg();
  file.seekg(0, file.beg);

  buffer_ = new char [length_];

  file.read(buffer_, length_);

  file.close();

  skipSpaces();

  // Begin JSON file
  switch (buffer_[current_]) {

    case '{': *root = decodeObject(); break;
    case '[': *root = decodeArray();  break;
    case '"': *root = decodeString(); break;
    default:  *root = decodeValue();  break;
  }

  return true;
}

//------------------------------------------------------------------------------
void JsonReader::skipSpaces() {

  while (current_ < length_) {
    char c = buffer_[current_];
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
      current_++;
    else
      break;
  }

  if (strncmp(&buffer_[current_], COMMENT_LINE_INDICATOR, 2) == 0) {
    skipCommentLine();
  }
}

//------------------------------------------------------------------------------
void JsonReader::skipCommentLine() {

  while (current_ < length_) {
    char c = buffer_[current_];
    if (c == '\r' || c == '\n') break;
    else current_++;
  }

  skipSpaces();
}

//------------------------------------------------------------------------------
uint32_t JsonReader::endOfString() {

  uint32_t end = current_;

  while ((
      buffer_[end] != '"' ||
      strncmp(&buffer_[end-1], ESCAPED_DOUBLEQUOTES, 2) == 0) &&
    end < length_) {

    end++;
  }

  return end;
}

//------------------------------------------------------------------------------
string JsonReader::decodeKey() {

  current_++;

  uint32_t keyStart = current_;
  uint32_t keyEnd = endOfString();
  uint32_t keyLength = keyEnd-keyStart;

  char *keyBuffer = new char [keyLength+1];

  strncpy(keyBuffer, &buffer_[current_], keyLength);
  keyBuffer[keyLength] = '\0';

  string key(keyBuffer);
  // TODO escape invalid characters
  delete[] keyBuffer;

  current_ = keyEnd;
  // Skip closing double quote
  current_++;

  return key;
}

//------------------------------------------------------------------------------
JsonObject *JsonReader::decodeObject() {

  JsonObject *object = new JsonObject();

  current_++;

  while (current_ < length_) {

    // "key" : value
    skipSpaces();

    // first check whether it is empty ...
    if (buffer_[current_] == '}') {
      DEBUG("Empty value (%u)", current_);
      break;
    }

    string key = decodeKey();

    skipSpaces();

    // We should find a colon at current_ position
    if (buffer_[current_] == ':') {
      current_++;
      skipSpaces();
    } else {
      DEBUG("Unable to process object at key(%s)", key.c_str());
      break;
    }

    switch (buffer_[current_]) {
      case '{': object->push(key, decodeObject());  break;
      case '[': object->push(key, decodeArray());   break;
      case '"': object->push(key, decodeString());  break;
      case 't': // true
      case 'f': // false
        object->push(key, decodeBoolean());         break;
      default:  object->push(key, decodeNumber());  break;
    }

    skipSpaces();

    if (buffer_[current_] == '}') break;
    if (buffer_[current_] == ',') current_++;
  }

  // skip closing object bracket
  current_++;

  return object;
}

//------------------------------------------------------------------------------
JsonArray *JsonReader::decodeArray() {

  JsonArray *array = new JsonArray();

  current_++;

  while (current_ < length_) {

    skipSpaces();

    switch (buffer_[current_]) {
      case '{': array->push(decodeObject());  break;
      case '[': array->push(decodeArray());   break;
      case '"': array->push(decodeString());  break;
      case 't': // true
      case 'f': // false
        array->push(decodeBoolean());         break;
      default:  array->push(decodeNumber());  break;
    }

    skipSpaces();

    if (buffer_[current_] == ']') break;
    if (buffer_[current_] == ',') current_++;
  }

  // skip closing array bracket
  current_++;

  return array;
}

//------------------------------------------------------------------------------
JsonString *JsonReader::decodeString() {

  current_++;

  uint32_t end = current_;
  uint32_t length = 0;

  JsonString *string = NULL;

  end = endOfString();

  length = end-current_;

  char *buffer = new char [length+1];

  strncpy(buffer, &buffer_[current_], length);
  buffer[length] = '\0';
  current_ = end;

  string = new JsonString(buffer);

  // Skip closing double quote
  current_++;

  delete[] buffer;

  return string;
}

//------------------------------------------------------------------------------
JsonNumber *JsonReader::decodeNumber() {

  uint32_t end = current_;
  uint32_t length = 0;

  while (strchr(VALID_NUMBER_CHARS, buffer_[end]) != NULL) {
    end++;
  }

  length = end - current_;

  char *buffer = new char [length+1];

  strncpy(buffer, &buffer_[current_], length);
  buffer[length] = '\0';

  JsonNumber *number = new JsonNumber(strtod(buffer, NULL));

  delete [] buffer;

  current_ = end;

  return number;
}

//------------------------------------------------------------------------------
JsonBoolean *JsonReader::decodeBoolean() {

  JsonBoolean *boolean = NULL;

  if (strncmp(&buffer_[current_], VALID_TRUE, 4) == 0) {
    boolean = new JsonBoolean(true);
    current_ += 5;
  } else if (strncmp(&buffer_[current_], VALID_FALSE, 5) == 0) {
    boolean = new JsonBoolean(false);
    current_ += 6;
  } else {
    DEBUG("Unable to decode boolean");
    current_++;
  }

  return boolean;
}

//------------------------------------------------------------------------------
JsonValue *JsonReader::decodeValue() {
  return NULL;
}
