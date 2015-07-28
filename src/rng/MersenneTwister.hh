//
// C++ adaptation of Mersenne Twister RNG from MT19937-64 by Takuji Nishimura
// and Makoto Matsumoto.
//
// http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/VERSIONS/C-LANG/mt19937-64.c
//
#ifndef __MERSENNETWISTER_HH__
#define __MERSENNETWISTER_HH__

#include "Object.hh"

#define NN 312
#define MM 156
#define MATRIX_A 0xB5026F5AA96619E9ULL
#define UM 0xFFFFFFFF80000000ULL /* Most significant 33 bits */
#define LM 0x7FFFFFFFULL /* Least significant 31 bits */

class MersenneTwister : public Object {

 private:

 protected:

   // The array for the state vector
   unsigned long long mt_[NN];

   // mti==NN+1 means mt[NN] is not initialized
   int mti_;

 public:

  MersenneTwister() {

    unsigned long long init[4] = {
      0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL};
    unsigned long long length = 4;

    mti_ = NN+1;
    init_by_array64(init, length);
  }

  MersenneTwister(unsigned long long init_key[], unsigned long long key_length) {
    mti_ = NN+1;
    init_by_array64(init_key, key_length);
  }

  MersenneTwister(unsigned long long seed) {
    mti_ = NN+1;
    init_genrand64(seed);
  }

  ~MersenneTwister() { }

  void init_genrand64(unsigned long long seed);

  void init_by_array64(unsigned long long init_key[], unsigned long long key_length);

  unsigned long long genrand64_int64(void);

  /* generates a random number on [0, 2^63-1]-interval */
  long long genrand64_int63(void) {
    return (long long)(genrand64_int64() >> 1);
  }

  /* generates a random number on [0,1]-real-interval */
  double genrand64_real1(void) {
    return (genrand64_int64() >> 11) * (1.0/9007199254740991.0);
  }

  /* generates a random number on [0,1)-real-interval */
  double genrand64_real2(void) {
    return (genrand64_int64() >> 11) * (1.0/9007199254740992.0);
  }

  /* generates a random number on (0,1)-real-interval */
  double genrand64_real3(void) {
    return ((genrand64_int64() >> 12) + 0.5) * (1.0/4503599627370496.0);
  }
};

#endif
