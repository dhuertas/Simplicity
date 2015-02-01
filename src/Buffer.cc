#include "Buffer.hh"

Buffer::Buffer() : 
  raw_(NULL),
  size_(0),
  readPos_(0),
  writePos_(0) {

  raw_ = (uint8_t *)malloc(BUFFER_SIZE);
  size_ = BUFFER_SIZE;
}

//------------------------------------------------------------------------------
Buffer::~Buffer() {

  size_ = 0;
  readPos_ = 0;
  writePos_ = 0;

  free(raw_);
  raw_ = NULL;
}

//------------------------------------------------------------------------------
uint8_t *Buffer::allocate(uint32_t newSize) {

  uint8_t *ptr = NULL;

  if (size_ < newSize) {
    ptr = (uint8_t *)realloc(raw_, newSize);
  }

  if (ptr == NULL) {
    ERROR("Unable to reallocate buffer");
    return NULL;
  }

  raw_ = ptr;
  size_ = newSize;
  return ptr;
}

//------------------------------------------------------------------------------
uint32_t Buffer::append(short value) {

  if (writePos_ + sizeof(value) > size_) {
    allocate(writePos_ + sizeof(value));
  }

  memcpy(&raw_[writePos_], &value, sizeof(value));

  return sizeof(value);
}

//------------------------------------------------------------------------------
uint32_t Buffer::append(int value) {

  if (writePos_ + sizeof(value) > size_) {
    allocate(writePos_ + sizeof(value));
  }

  memcpy(&raw_[writePos_], &value, sizeof(value));

  return sizeof(value);
}

//------------------------------------------------------------------------------
uint32_t Buffer::append(float value) {

  if (writePos_ + sizeof(value) > size_) {
    allocate(writePos_ + sizeof(value));
  }

  memcpy(&raw_[writePos_], &value, sizeof(value));

  return sizeof(value);
}

//------------------------------------------------------------------------------
uint32_t Buffer::append(double value) {

  if (writePos_ + sizeof(value) > size_) {
    allocate(writePos_ + sizeof(value));
  }

  memcpy(&raw_[writePos_], &value, sizeof(value));

  return sizeof(value);
}

//------------------------------------------------------------------------------
uint32_t Buffer::append( uint8_t value) {

  if (writePos_ + sizeof(value) > size_) {
    allocate(writePos_ + sizeof(value));
  }

  memcpy(&raw_[writePos_], &value, sizeof(value));

  return sizeof(value);
}

//------------------------------------------------------------------------------
uint32_t Buffer::append(uint16_t value) {

  if (writePos_ + sizeof(value) > size_) {
    allocate(writePos_ + sizeof(value));
  }

  memcpy(&raw_[writePos_], &value, sizeof(value));

  return sizeof(value);
}

//------------------------------------------------------------------------------
uint32_t Buffer::append(uint32_t value) {

  if (writePos_ + sizeof(value) > size_) {
    allocate(writePos_ + sizeof(value));
  }

  memcpy(&raw_[writePos_], &value, sizeof(value));

  return sizeof(value);
}

//------------------------------------------------------------------------------
uint32_t Buffer::append(uint64_t value) {

  if (writePos_ + sizeof(value) > size_) {
    allocate(writePos_ + sizeof(value));
  }

  memcpy(&raw_[writePos_], &value, sizeof(value));

  return sizeof(value);
}

//------------------------------------------------------------------------------
uint32_t Buffer::append(uint8_t *buf, uint32_t length) {

  uint8_t *ptr = NULL;

  if (writePos_ + length > size_) {

    ptr = allocate(2*size_ < MAX_REALLOC_SIZE ? 2*size_ : size_ + MAX_REALLOC_SIZE);

    if (ptr == NULL) {
      DEBUG("Unable to append to buffer");
      return 0;
    }

    memcpy(&raw_[writePos_], buf, length);
    writePos_ += length;
    return length;
  }

  memcpy(&raw_[writePos_], buf, length);
  writePos_ += length;
  return length;
  
}

//------------------------------------------------------------------------------
uint32_t Buffer::append(char *buf, uint32_t length) {

  DEBUG("Buffer(%s)", raw_);
  return append((uint8_t *)buf, length);
}

//------------------------------------------------------------------------------
uint32_t Buffer::append(const char *buf, uint32_t length) {

  return append((uint8_t *)buf, length);
}

//------------------------------------------------------------------------------
uint32_t Buffer::append(Buffer *buffer) {

  if (buffer != NULL) {
    ERROR("Null buffer");
    return 0;
  }

  uint8_t *raw = buffer->getRaw();
  uint32_t length = buffer->length();

  return append(raw, length);
}

//------------------------------------------------------------------------------
uint8_t *Buffer::at(uint32_t pos) {

  if (pos <= writePos_) {
    return &raw_[pos];
  }

  return NULL;
}