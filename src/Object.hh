#ifndef __OBJECT_HH__
#define __OBJECT_HH__

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <cstring>

#include "Log.hh"

class Object {

 private:

 protected:

 public:

  static uint32_t count;

  Object() {
    count++;
  }

  ~Object() {
    count--;
  }
};

#endif