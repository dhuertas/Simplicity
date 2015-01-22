#include "Time.hh"
#include "Log.hh"

int Time::scaleexp_ = Time::SCALEEXP_UNINITIALIZED;
int64_t Time::dscale_;
double Time::fscale_;
double Time::invfscale_;

//------------------------------------------------------------------------------
void Time::setScaleExp(int e) {

  if (e == scaleexp_)
      return;

  if (scaleexp_ != SCALEEXP_UNINITIALIZED) {
    ERROR("Attempt to change the scale exponent after initialization");
    return;
  }

  if (e < -18 || e > 0) {
    ERROR("Time scale exponent %d is out of accepted range -18..0; "
      "recommended value is -12 (picosecond resolution with range +-106 days)", e);
    return;
  }

  // calculate 10^-e
  scaleexp_ = e;
  int64_t scale = 1;
  while (e++ < 0)
      scale *= 10;

  // store it in double too
  dscale_ = scale;
  fscale_ = (double )scale;
  invfscale_ = 1.0 / fscale_;

}

//------------------------------------------------------------------------------
int64_t Time::toInt64(double d) {

  if (fabs(d) > INT64_MAX_DBL) {
    ERROR("Cannot convert %g to Time: out of range [-%g, +%g], allowed by scale exponent %d",
      d*invfscale_, INT64_MAX*invfscale_, INT64_MAX*invfscale_, scaleexp_);
    return -1;
  }

  return (int64_t)d;
}

//------------------------------------------------------------------------------
int64_t Time::cmp(Time *a, Time *b) {
  return a->raw() - b->raw();
}

//------------------------------------------------------------------------------
int64_t Time::cmp(Time *a, double b) {
  return a->raw() - toInt64(fscale_*b);
}

//------------------------------------------------------------------------------
int64_t Time::cmp(Time *a, int64_t b) {
  return a->raw() - b;
}

//------------------------------------------------------------------------------
int64_t Time::cmp(Time *a, int b) {
  return a->raw() - (uint64_t )(fscale_*b);
}

//------------------------------------------------------------------------------
Time operator+ (Time &t1, Time &t2) {
  return Time(t1.raw() + t2.raw());
}

//------------------------------------------------------------------------------
bool operator> (Time &t1, Time &t2) {
  return t1.raw() > t2.raw();
}

//------------------------------------------------------------------------------
bool operator>=(Time &t1, Time &t2) {
  return t1.raw() >= t2.raw();
}

//------------------------------------------------------------------------------
bool operator< (Time &t1, Time &t2) {
  return t1.raw() < t2.raw();
}

//------------------------------------------------------------------------------
bool operator<=(Time &t1, Time &t2) {
  return t1.raw() <= t2.raw();
}