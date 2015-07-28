#include "MersenneTwister.hh"

// -----------------------------------------------------------------------------
void MersenneTwister::init_genrand64(unsigned long long seed) {

  mt_[0] = seed;
  for (mti_ = 1; mti_ < NN; mti_++)
    mt_[mti_] =  (6364136223846793005ULL * (mt_[mti_ - 1] ^ (mt_[mti_ - 1] >> 62)) + mti_);
}

// -----------------------------------------------------------------------------
void MersenneTwister::init_by_array64(
  unsigned long long init_key[],
  unsigned long long key_length) {

  unsigned long long i, j, k;
  init_genrand64(19650218ULL);

  i = 1;
  j = 0;
  k = (NN > key_length ? NN : key_length);

  for (; k; k--) {
    mt_[i] = (mt_[i] ^ ((mt_[i-1] ^ (mt_[i-1] >> 62)) * 3935559000370003845ULL))
      + init_key[j] + j; /* non linear */
    i++; j++;
    if ( i>= NN) { mt_[0] = mt_[NN-1]; i=1; }
    if (j >= key_length) j = 0;
  }

  for (k = NN-1; k; k--) {
    mt_[i] = (mt_[i] ^ ((mt_[i-1] ^ (mt_[i-1] >> 62)) * 2862933555777941757ULL))
      - i; /* non linear */
    i++;
    if (i >= NN) { mt_[0] = mt_[NN-1]; i = 1; }
  }

  mt_[0] = 1ULL << 63; /* MSB is 1; assuring non-zero initial array */
}

// -----------------------------------------------------------------------------
unsigned long long MersenneTwister::genrand64_int64(void) {

  int i;
  unsigned long long x;
  static unsigned long long mag01[2] = {0ULL, MATRIX_A};

  if (mti_ >= NN) { /* generate NN words at one time */

      /* if init_genrand64() has not been called, */
      /* a default initial seed is used     */
      if (mti_ == NN+1)
          init_genrand64(5489ULL);

      for (i = 0; i < NN-MM; i++) {
          x = (mt_[i]&UM) | (mt_[i+1]&LM);
          mt_[i] = mt_[i+MM] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
      }
      for (; i < NN-1; i++) {
          x = (mt_[i]&UM)|(mt_[i+1]&LM);
          mt_[i] = mt_[i+(MM-NN)] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
      }
      x = (mt_[NN-1]&UM) | (mt_[0]&LM);
      mt_[NN-1] = mt_[MM-1] ^ (x>>1) ^ mag01[(int)(x&1ULL)];

      mti_ = 0;
  }

  x = mt_[mti_++];

  x ^= (x >> 29) & 0x5555555555555555ULL;
  x ^= (x << 17) & 0x71D67FFFEDA60000ULL;
  x ^= (x << 37) & 0xFFF7EEE000000000ULL;
  x ^= (x >> 43);

  return x;
}
