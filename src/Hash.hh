#ifndef __HASH_HH__
#define __HASH_HH__

#include <cstdint>
#include <cstddef>

uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed);

#endif
