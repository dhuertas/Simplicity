#include "Log.hh"

int Logger::level_ = 1;

bool Logger::redirected_ = false;

const char *levelStr_[] = {
  "NONE",
  "INFO",
  "DEBUG",
  "PEDANTIC"
};

FILE* Logger::f_ = NULL;

//------------------------------------------------------------------------------
Logger::~Logger() {

  if (redirected_) {
    fclose(stderr);
    f_ = NULL;
  }
}
