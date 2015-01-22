#ifndef __JSONREADER_HH__
#define __JSONREADER_HH__

#include "Object.hh"
#include "JsonValue.hh"
#include "JsonBoolean.hh"
#include "JsonObject.hh"
#include "JsonArray.hh"
#include "JsonString.hh"
#include "JsonNumber.hh"

#define ESCAPED_DOUBLEQUOTES    "\\\""
#define COMMENT_LINE_INDICATOR  "//"

using namespace std;

class JsonReader : public Object {

 private:

 protected:

  uint32_t current_;

  uint32_t length_;

  char *buffer_;

 public:

  JsonReader();

  ~JsonReader();

  void reset();

  void skipSpaces();

  void skipCommentLine();

  uint32_t endOfString();

  bool parse(const char *fileName, JsonValue **root);

  string decodeKey(void);

  JsonObject *decodeObject(void);

  JsonArray *decodeArray(void);

  JsonString *decodeString(void);

  JsonNumber *decodeNumber(void);

  JsonBoolean *decodeBoolean(void);

  JsonValue *decodeValue(void);

};

#endif