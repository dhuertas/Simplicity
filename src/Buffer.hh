#ifndef __BUFFER_HH__
#define __BUFFER_HH__

#include "Object.hh"

#define BUFFER_SIZE       2048
#define MAX_REALLOC_SIZE  100*BUFFER_SIZE

class Buffer : public Object {

 private:

  uint8_t *raw_;

 protected:

  uint32_t size_;

  uint32_t readPos_;

  uint32_t writePos_;

 public:

  Buffer();

  ~Buffer();

  uint8_t *allocate(uint32_t size);

  uint32_t append(short value);

  uint32_t append(int value);

  uint32_t append(float value);

  uint32_t append(double value);

  uint32_t append( uint8_t value);

  uint32_t append(uint16_t value);

  uint32_t append(uint32_t value);

  uint32_t append(uint64_t value);

  uint32_t append(char *buf, uint32_t len);

  uint32_t append(const char *buf, uint32_t len);

  uint32_t append(uint8_t *buf, uint32_t len);

  uint32_t append(Buffer *buffer);

  uint8_t *at(uint32_t pos);

  void clear() { writePos_ = 0; };

  void reset(uint8_t value = 0) { memset(raw_, value, size_); }

  uint32_t length() { return writePos_; }

  uint8_t *getRaw() { return raw_; }

};

#endif