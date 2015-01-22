#ifndef __TIME_HH__
#define __TIME_HH__

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "Log.hh"

// the most positive int64 value, represented as double
#define INT64_MAX_DBL  9.22337203685e18

enum TimeUnit {
  TIME_S  =   0,
  TIME_MS =  -3,
  TIME_US =  -6,
  TIME_NS =  -9,
  TIME_PS = -12,
  TIME_FS = -15
};

// Similar to Omnet SimTime class
class Time {

 private:

  static int scaleexp_;
  static int64_t dscale_;
  static double fscale_;

  // 1/fscale; we store it because floating-point multiplication is faster than division
  static double invfscale_;

  // For now we store the time as a double...
  int64_t t_;

 protected:

 public:

  static const int SCALEEXP_UNINITIALIZED = 0xffff;

  Time() { t_ = 0; };

  Time(double t) { t_ = toInt64(fscale_*t); };

  Time(int64_t t) { t_ = t; }

  Time(int t) { t_ = (int64_t )(fscale_*t); }

  double dbl() { return t_*invfscale_; }

  int64_t raw() { return t_; }

  // TODO check for overflow
  static Time add(Time *a, Time *b) { return Time(a->raw() + b->raw()); }

  // TODO check for overflow
  Time* add(Time *a) { t_ += a->raw(); return this; }

  // TODO check for overflow
  static Time sub(Time *a, Time *b) { return Time(a->raw() - b->raw()); }

  // TODO check for overflow
  Time* sub(Time *a) { t_ -= a->raw(); return this; }

  static int64_t cmp(Time *a, Time *b);

  static int64_t cmp(Time *a, double t);

  static int64_t cmp(Time *a, int64_t t);

  static int64_t cmp(Time *a, int t);  

  Time getTime() { return Time(t_); }

  void setTime(double t) { t_ = toInt64(fscale_*t); }

  void setRaw(int64_t t) { t_ = t; }

  static void setScaleExp(int e);

  static int64_t toInt64(double d);

  const Time& operator*=(double d) { t_ = toInt64(t_*d); return *this; }
  const Time& operator/=(double d) { t_ = toInt64(t_/d); return *this; }
  const Time& operator+=(const Time &t) { t_ += t.t_; return *this; }

  friend Time operator+ (Time &t1, Time &t2);

  friend bool operator> (Time &t1, Time &t2);
  friend bool operator>=(Time &t1, Time &t2);
  friend bool operator< (Time &t1, Time &t2);
  friend bool operator<=(Time &t1, Time &t2);

};

#endif
