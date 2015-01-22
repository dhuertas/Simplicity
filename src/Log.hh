#ifndef __LOG_HH__
#define __LOG_HH__

#include <cstdlib>
#include <stdio.h>

#define LOG_BUFFER_LENGTH 2048
#define LOG_FORMAT_LENGTH 512

#define INFO(fmt, ...) do { \
  if (Logger::level_ >= Logger::LOG_INFO) \
    fprintf(stderr, "[INFO][%s]: " fmt "\n", \
    __PRETTY_FUNCTION__, ##__VA_ARGS__); } while(0)

#define DEBUG(fmt, ...) do { \
  if (Logger::level_ >= Logger::LOG_DEBUG) \
    fprintf(stderr, "[DEBUG][%s:%d %s]: " fmt "\n", \
    __FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__); } while(0)

#define ERROR(fmt, ...) do { \
  fprintf(stderr, "[ERROR][%s:%d %s]: " fmt "\n", \
    __FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__); } while(0)

#define FCKIT(fmt, code, ...) do { \
  fprintf(stderr, "[FCKIT][%s:%d %s]: " fmt "\n", \
    __FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__); exit(code); } while(0)

class Logger {

 public:

  enum {
    LOG_NONE,
    LOG_INFO,
    LOG_DEBUG,
    LOG_PEDANTIC
  };

 private:

 protected:

 public:

  ~Logger();

  // Log level:
  //  0. none
  //  1. info
  //  2. debug
  //  3. pedantic
  static int level_;

  static const char *levelStr_[];

  static bool redirected_;

  static FILE *f_;

  Logger *getInstance() {
    static Logger instance;
    return &instance;
  }

  static int getLevel(void) {  return level_; }

  static void setLevel(int lvl) { level_ = lvl; }

  static void setRedirect(const char *fileName) {
    f_ = freopen(fileName, "w", stderr);
    redirected_ = true;
  }

};

#endif
