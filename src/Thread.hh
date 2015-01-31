#ifndef __THREAD_HH__
#define __THREAD_HH__

#include <pthread.h>

#include "Runnable.hh"

class Thread {

 private:

  pthread_t threadId_;

  pthread_attr_t threadAttribute_;

  Runnable *runnable_;

  bool detached_;

  void *result_;

  Thread(const Thread&);

  const Thread& operator=(const Thread&);

  void setCompleted();

  virtual void *run() { return result_; }

  static void *startThreadRunnable(void *pVoid);

  static void *startThread(void *pVoid);

 protected:

 public:

  Thread(Runnable *run, bool isDetached = false);

  Thread(bool isDetached = false);

  virtual ~Thread();

  void start();

  void *join();

};

#endif