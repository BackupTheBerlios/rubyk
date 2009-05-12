#ifndef _TEST_HELPER_H_
#define _TEST_HELPER_H_
#include <cxxtest/TestSuite.h>

#include "rubyk.h"
#include "mock/dummy_node.h"
#include "print.h"

#include <sys/timeb.h> // ftime
#include <ostream>

#define TEST_LIB_PATH "../lib"
#define TEST_FIXTURES_PATH "../test/fixtures"

void microsleep(size_t microseconds) {
  struct timespec sleeper;
  sleeper.tv_sec  = 0; 
  sleeper.tv_nsec = microseconds * 1000000;
  nanosleep (&sleeper, NULL);
}

#define assert_equal(x,y) _assert_equal(__FILE__,__LINE__,#y,x,y)
#define assert_true(e) _TS_ASSERT(__FILE__,__LINE__,e)
#define assert_false(e) _TS_ASSERT(__FILE__,__LINE__,!(e))

#define _OSCIT_ASSERT_EQUALS(f,l,cmd,x,y) { _TS_TRY { CxxTest::doAssertEquals( (f), (l), (cmd), (x), #y, (y), (0) ); } __TS_CATCH(f,l) }

class TestHelper : public CxxTest::TestSuite
{
protected:
  
  void _assert_equal(const char * file, int lineno, const char * descr, Real expected, Real found)
  {
    _OSCIT_ASSERT_EQUALS( file, lineno, TS_AS_STRING(descr), found, expected);
  }
  
  void _assert_equal(const char * file, int lineno, const char * descr, int expected, int found)
  {
    _OSCIT_ASSERT_EQUALS( file, lineno, TS_AS_STRING(descr), found, expected);
  }
  
  void _assert_equal(const char * file, int lineno, const char * descr, const char * expected, const char * found)
  {
    _OSCIT_ASSERT_EQUALS( file, lineno, TS_AS_STRING(descr), std::string(found), std::string(expected));
  }
  
  void _assert_equal(const char * file, int lineno, const char * descr, const char * expected, const std::string &found)
  {
    _OSCIT_ASSERT_EQUALS( file, lineno, TS_AS_STRING(descr), found, std::string(expected));
  }
  
  void _assert_equal(const char * file, int lineno, const char * descr, const std::string &expected, const std::string &found)
  {
    _OSCIT_ASSERT_EQUALS( file, lineno, TS_AS_STRING(descr), found, expected);
  }
  
  void _assert_equal(const char * file, int lineno, const char * descr, void * expected, void * found)
  {
    _OSCIT_ASSERT_EQUALS( file, lineno, TS_AS_STRING(descr), found, expected);
  }
};

std::string fixture_path(const char *path) {
  return std::string(TEST_FIXTURES_PATH).append("/").append(path);
}

#define assert_result(x,y) _assert_result(__FILE__,__LINE__,#y,x,y)
#define assert_print(x,y) _assert_print(__FILE__,__LINE__,#y,x,y)
#define assert_run(x,y) _assert_run(__FILE__,__LINE__,#y,x,y)

// ========================== ParseHelper  ====================== //

class ParseHelper : public TestHelper
{
public:
  ParseHelper() : planet_(NULL), output_(std::ostringstream::out), input_(std::istringstream::in) {}
    
  void setUp() {
    planet_ = new Planet;
    cmd_ = planet_->adopt_command(new TextCommand(input_, output_), false);
    planet_->call(LIB_URL, Value(TEST_LIB_PATH));
    output_.str(std::string("")); // clear output
    planet_->should_run(true);
  }
  
  void tearDown() {
    if (planet_) delete planet_;
    planet_ = NULL;
  }

protected:
  Planet * planet_;
  TextCommand * cmd_;
  std::ostringstream output_;
  std::ostringstream print_;
  std::istringstream input_;

  void setup_with_print(const char *input) {
    Object *object;
    cmd_->parse("p=Print()\nn=>p\n");
    object = planet_->object_at("/p");
    if (!object) {
      fprintf(stderr, "Print object '/p' not found !");
    } else {
      Print *print = TYPE_CAST(Print, object);
      if (print == NULL) {
        fprintf(stderr, "Wrong object type '%s'. Should be Print.\n", print->class_path());
      } else {
        print->set_output(&print_);
      }
    }
    cmd_->parse(input);
    if (output_.str().find("# 400 ") != std::string::npos ||
        output_.str().find("# 404 ") != std::string::npos ||
        output_.str().find("# 500 ") != std::string::npos) {
      // could find a better way to detect errors...
      fprintf(stderr, "Error while parsing '%s'\n%s\n", input, output_.str().c_str());
    }
    output_.str(std::string("")); // clear output
  }
  
  void setup(const char *input) {
    cmd_->parse(input);
    output_.str(std::string(""));
  }

  void _assert_result(const char *file, int lineno, const char *descr, const char *expected, const char *input) {
    output_.str(std::string("")); // clear output
    cmd_->parse(input);
    _OSCIT_ASSERT_EQUALS( file, lineno, TS_AS_STRING(descr), output_.str(), expected);
  }

  void _assert_print(const char * file, int lineno, const char *descr, const char *expected, const char *input) { 
    print_.str(std::string("")); // clear output
    cmd_->parse(input);
    _OSCIT_ASSERT_EQUALS( file, lineno, TS_AS_STRING(descr), print_.str(), expected);
  }

  void _assert_run(const char * file, int lineno, const char *descr, const char *expected, time_t length)
  {
    time_t start = planet_->current_time();
    print_.str(std::string("")); // clear output
    while(planet_->current_time() <= start + length && planet_->loop())
      ;
    _OSCIT_ASSERT_EQUALS( file, lineno, TS_AS_STRING(descr), print_.str(), expected);
  }
//  void clean_assert_result(const char * input, const char * pOutput)
//  {
//    clean_start();
//    assert_result(input, pOutput);
//  }
//
//  void clean_assert_bang(const char * input, const char * pOutput)
//  {
//    setup_with_print(input);
//    assert_bang("\n", pOutput);
//  }
//
//  void assert_bang(const char * input, const char * pOutput)
//  { 
//    worker_.unlock();
//    output_.str(std::string("")); // clear output
//    cmd_->set_silent(true);
//    cmd_->parse(input);
//    cmd_->parse("\nn.bang\n");
//    cmd_->set_silent(false);
//    worker_.lock();
//    TS_ASSERT_EQUALS( output_.str(), std::string(pOutput));
//  }
//
//  void clean_assert_print(const char * input, const char * pOutput)
//  {
//    clean_start();
//    assert_print(input, pOutput);
//
//  }
//
//  void clean_assert_run(time_t pLength, const char * input, const char * pOutput)
//  {
//    clean_start();
//    assert_run(pLength, input, pOutput);
//  }
//
};

#endif // _TEST_HELPER_H_