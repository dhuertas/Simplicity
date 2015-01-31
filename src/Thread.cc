#include "Thread.hh"
#include "Log.hh"

// -----------------------------------------------------------------------------
Thread::Thread(Runnable *runnable, bool isDetached) : 
  runnable_(runnable),
  detached_(isDetached),
  result_(NULL) {

  if ( ! runnable) {
    ERROR("runnable is NULL");
  }
}

// -----------------------------------------------------------------------------
Thread::Thread(bool isDetached) : 
  runnable_(NULL),
  detached_(isDetached) {

}

// -----------------------------------------------------------------------------
Thread::~Thread() {

}

// -----------------------------------------------------------------------------
void *Thread::startThreadRunnable(void *pVoid) {

  Thread *runnableThread = static_cast<Thread*>(pVoid);

  if ( ! runnableThread) FCKIT("Oops", -1);

  runnableThread->result_ = runnableThread->runnable_->run();
  runnableThread->setCompleted();

  return runnableThread->result_;
}

// -----------------------------------------------------------------------------
void *Thread::startThread(void *pVoid) {

  Thread *thread = static_cast<Thread*>(pVoid);

  if ( ! thread) FCKIT("Oops", -1);

  thread->result_ = thread->run();
  thread->setCompleted();

  return thread->result_;
}

// -----------------------------------------------------------------------------
void Thread::start() {

  int status = pthread_attr_init(&threadAttribute_);

  if (status) FCKIT("pthread_attr_init failed", status);

  status = pthread_attr_setscope(&threadAttribute_, PTHREAD_SCOPE_SYSTEM);

  if (status) FCKIT("pthread_attr_setscope failed", status);

  if (detached_) {
    status = pthread_attr_setdetachstate(&threadAttribute_, PTHREAD_CREATE_DETACHED);
    if (status) FCKIT("pthread_attr_setdetachstate failed", status);    
  }

  if ( ! runnable_) {
    status = pthread_create(&threadId_, &threadAttribute_, Thread::startThread, (void *)this);
    if (status) FCKIT("pthread_create failed", status);
  } else {
    status = pthread_create(&threadId_, &threadAttribute_, Thread::startThreadRunnable, (void *)this);
    if (status) FCKIT("pthread_create failed", status);
  }

  status = pthread_attr_destroy(&threadAttribute_);

  if (status) FCKIT("pthread_attr_destroy failed", status);
}

// -----------------------------------------------------------------------------
void *Thread::join() {

  int status = pthread_join(threadId_, NULL);
  if (status) FCKIT("pthread_join failed", status);

  return result_;
}

// -----------------------------------------------------------------------------
void Thread::setCompleted() {
  // completion handled by pthread_join()
}