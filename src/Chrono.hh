#ifndef __CHRONO_HH__
#define __CHRONO_HH__

#include <sys/time.h>

#include "Object.hh"

class Chrono : public Object {

 private:

 protected:

  timeval start_;

 public:

  void start() {  gettimeofday(&start_, NULL); }

  double elapsed() {
    timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec-start_.tv_sec+(now.tv_usec-start_.tv_usec)/1000000.0;
  }

  timeval getStartTime() { return start_; }

  timeval getCurrentTime() {
    timeval res;
    gettimeofday(&res, NULL);
    return res;
  }

};

#endif