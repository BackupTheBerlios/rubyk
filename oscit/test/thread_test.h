#include "test_helper.h"
#include "oscit/thread.h"
#include <sstream>

struct DummyWorker
{
  DummyWorker() : value_(0) {}
  
  void count(Thread *runner) {
    while (runner->should_run()) {
      runner->lock();
      ++value_;
      runner->unlock();
      
      millisleep(10);
    }
  }
  
  void count_twice(Thread *runner) {
    while (runner->should_run() && value_ < 2) {
      runner->lock();
      ++value_;
      runner->unlock();
      
      millisleep(10);
    }
  }
  
  void count_high(Thread *runner) {
    runner->high_priority();
    while (runner->should_run()) {
      runner->lock();
      ++value_;
      runner->unlock();
      
      millisleep(10); // should be interrupted
    }
  }
  
  void read(Thread *runner) {
    int i = 0;
    while (runner->should_run()) {
      runner->lock();
      std::cin >> i;
      value_ += i;
      runner->unlock();
    }
  }
  
  int  value_;
};

struct DummySubWorker : public Thread
{
  DummySubWorker() : value_(0) {}
  
  ~DummySubWorker() {
    kill();
  }
  
  void count(Thread *runner) {
    while (should_run()) {
      lock();
      ++value_;
      unlock();
      
      millisleep(10);
    }
  }
  
  int  value_;
};

class ThreadTest : public TestHelper
{  
public:
  void test_create_delete( void ) {
    DummyWorker counter;
    Thread *runner = new Thread;
    runner->start<DummyWorker, &DummyWorker::count>(&counter, NULL);
    // let it run 2 times: +1 ... 10ms ... +1 ... 5ms .. kill [end]
    millisleep(15);
    delete runner;
    // should join here
    assert_equal(2, counter.value_);
  }
  
  void test_create_kill_sub_class( void ) {
    DummySubWorker * counter = new DummySubWorker;
    counter->start<DummySubWorker, &DummySubWorker::count>(counter, NULL);
    // let it run 2 times: +1 ... 10ms ... +1 ... 5ms .. kill [end]
    millisleep(15);
    counter->kill();
    // should join here
    assert_equal(2, counter->value_);
    delete counter;
    // should not block
  }
  
  void test_create_delete_sub_class( void ) {
    DummySubWorker * counter = new DummySubWorker;
    counter->start<DummySubWorker, &DummySubWorker::count>(counter, NULL);
    // let it run 2 times: +1 ... 10ms ... +1 ... 5ms .. kill [end]
    millisleep(15);
    delete counter;
    // should join here
    // should not block
  }
  
  void test_create_stop( void ) {
    DummyWorker counter;
    Thread *runner = new Thread;
    runner->start<DummyWorker, &DummyWorker::count>(&counter, NULL);
    // let it run 2 times: +1 ... 10ms ... +1 ... 5ms .. kill .. 5ms [end]
    millisleep(15);
    runner->stop();
    runner->stop(); // should not lock
    
    // should join here
    assert_equal(2, counter.value_);
    
    delete runner;
  }
  
  // This version sends a SIGINT to stop the thread
  void test_create_kill( void ) {
    DummyWorker counter;
    Thread *runner = new Thread;
    runner->start<DummyWorker, &DummyWorker::count>(&counter, NULL);
    // let it run 1 times: +1 ... 5ms kill [end]
    millisleep(5);
    runner->kill();
    runner->kill(); // should not lock
    // should join here
    assert_equal(1, counter.value_);
    
    delete runner;
  }
  
  void test_create_restart( void ) {
    DummyWorker counter;
    Thread *runner = new Thread;
    runner->start<DummyWorker, &DummyWorker::count>(&counter, NULL);
    // let it run 2 times: +1 ... 10ms ... +1 ... 5ms .. kill .. 5ms [end]
    millisleep(15);
    runner->kill();
    
    runner->start<DummyWorker, &DummyWorker::count>(&counter, NULL);
    // let it run 2 times: +1 ... 10ms ... +1 ... 5ms .. kill .. 5ms [end]
    millisleep(15);
    runner->kill();
    
    // should join here
    assert_equal(4, counter.value_);
    
    delete runner;
  }
  
  void test_create_high_priority( void ) {
    DummyWorker counter;
    Thread *runner = new Thread;
    runner->start<DummyWorker, &DummyWorker::count_high>(&counter, NULL);
    // let it run 2 times: +1 ... 10ms ... +1 ... 5ms .. kill .. 5ms [end]
    millisleep(15);
    delete runner;
    // should join here
    assert_equal(2, counter.value_);
  }
  
  // test fails until we have a good solution: see doc/prototypes/term_readline.cpp
  // void test_kill_readline( void ) {
  //   DummyWorker counter;
  //   Thread *runner = new Thread;
  //   runner->start<DummyWorker, &DummyWorker::read>(&counter, NULL);
  //   // reads input...
  //   millisleep(10);
  //   runner->kill();
  //   // should interrupt
  //   assert_equal(0, counter.value_);
  //   delete runner;
  // }
  
  void test_join_without_kill( void ) {
    DummyWorker counter;
    Thread *runner = new Thread;
    runner->start<DummyWorker, &DummyWorker::count_twice>(&counter, NULL);
    // let it run 2 times: +1 ... 10ms ... +1 ... 10ms [done]
    runner->join();
    // should join here
    assert_equal(2, counter.value_);
    
    delete runner;
  }
};