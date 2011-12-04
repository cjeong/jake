/**
 * @file java_thread.h
 * @desc Java threads and thread scheduler
 *
 * @author cjeong
 */
#ifndef JAVA_THREAD_H
#define JAVA_THREAD_H


enum JavaThreadStateE {
  JavaRunnable,
  JavaWaiting,
  JavaRunning
};

enum JavaThreadLocationE {
  JavaInHW,
  JavaInSW
};

class JavaThreadI {
public:

public:
  /* the state of the thread */
  virtual JavaThreadStateE getThreadState() = 0;

  /* where this thread is executed at this point */
  virtual JavaThreadLocationE getThreadLocation() = 0;
};


class JavaThreadSchedulerI {
public:
  /* add the thread to the scheduler */
  virtual void addThread(JavaThread *) = 0;

  /* get the number of threads in the scheduler */
  virtual void getNumThreads() = 0;

  /* returns the next runnable thread with the highest priority */
  virtual JavaThreadI *pickNextThread() = 0;
};

#endif /* JAVA_THREAD_H */
