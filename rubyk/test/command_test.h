#include "test_helper.h"

class CreateCommandTest : public CxxTest::TestSuite
{
public:
  void test_create( void ) 
  {
    Planet planet;
    std::istringstream  input(std::istringstream::in);   // allow input operations
    std::ostringstream output(std::ostringstream::out);  // allow output  operations
    Command cmd(input, output);
    planet.listen_to_command(cmd);
    time_t start = planet.mCurrentTime;
    while((planet.mCurrentTime <= start + 50) && planet.do_run())
      ;
    
    TS_ASSERT_EQUALS( output.str(), std::string("Welcome to rubyk !\n\n"));
  }
};

class ParseCommandTest : public ParseHelper
{
public:
  
  void test_parse_command( void ) 
  { assert_result("v1=Number(value:1)\n","<Number:/v1 1.00>\n"); }
  
  void test_parse_zero( void ) 
  { assert_result("v1=Number(0)\n","<Number:/v1 0.00>\n"); }
  
  void test_parse_zero_dot_int( void ) 
  { assert_result("v1=Number(0.1)\n","<Number:/v1 0.10>\n"); }
  
  void test_parse_command_with_spaces( void ) 
  { assert_result("v1 = Number(2)\n\n","<Number:/v1 2.00>\n"); }
  
  void test_parse_multiline_string( void ) 
  { assert_result("t=Test(\"I love you (a lot)\nwhen you're naked...\")\n","<Test:/t \'I love you (a lot)\nwhen you\'re naked...\' counter:0>\n"); }
  
  void test_parse_integer( void ) 
  { assert_result("v1 = Number(2.35)\n\n","<Number:/v1 2.35>\n"); }
  
  void test_parse_real( void ) 
  { assert_result("v1 = Number(2.35)\n\n","<Number:/v1 2.35>\n"); }
  
  void test_parse_negative_integer( void ) 
  { assert_result("v1 = Number(-5)\n\n","<Number:/v1 -5.00>\n"); }
  
  void test_parse_negative_real( void ) 
  { assert_result("v1 = Number(-2.35)\n\n","<Number:/v1 -2.35>\n"); }
  
  void test_execute_method( void ) 
  { assert_result("v1=Counter(3)\nv1.bang\nv1\n","<Counter:/v1 3.00 (+1.00)>\n<Counter:/v1 4.00 (+1.00)>\n"); }
  
  void test_inspect_command( void ) 
  { assert_print("i=Counter(14)\ni\n","<Counter:/i 14.00 (+1.00)>\n"); }
  
  void test_syntax_error( void ) 
  { assert_result("i=Counter(1)\n4\ni\n","<Counter:/i 1.00 (+1.00)>\nSyntax error near '\n4\ni\n'.\n<Counter:/i 1.00 (+1.00)>\n"); }
  
  void test_execute_accessor_with_params( void ) 
  { assert_result("i=Counter(1)\ni.increment(5)\ni.bang\ni\n","<Counter:/i 1.00 (+1.00)>\n5.00\n<Counter:/i 6.00 (+5.00)>\n"); }
  
  void test_execute_set( void ) 
  { assert_result("i=Counter(1)\ni.set(counter:4 increment:5)\ni.bang\ni\n","<Counter:/i 1.00 (+1.00)>\n<Counter:/i 9.00 (+5.00)>\n"); }
  
  void test_execute_set_value( void ) 
  { assert_result("i=Counter(1)\ni.set(5)\ni.bang\ni\n","<Counter:/i 1.00 (+1.00)>\n<Counter:/i 6.00 (+1.00)>\n"); }
  
  void test_bang_shortcut( void ) 
  { assert_result("i=Counter(1)\ni.b\ni\n","<Counter:/i 1.00 (+1.00)>\n<Counter:/i 2.00 (+1.00)>\n"); }
  
  void test_execute_class_method( void ) 
  { assert_result("Test.hello\n","Hello: Nil\n"); }

  void test_parse_link( void ) 
  { 
    assert_result("n.out=>in.p\n",""); // just make sure there is no parse error
  }

  void test_parse_unLink( void ) 
  { 
    setup_with_print("n=Number(1)\n");
    assert_print("n // p\nn.bang()\n","");
  }
  
  void test_print( void )
  {
    setup_with_print("n=Number(7)\n");
    assert_print("n.bang()\n","7.00\n");
  }

  void test_parse_first_links( void ) 
  { assert_result("n=>p\n",""); }
  
  void test_parse_bad_links( void ) 
  { assert_result("n=>\n","Syntax error near '>\n'.\n"); }
  
  void test_create_same_name( void ) 
  { // It is not very clear what we should have here. I think replacing is not an option.
    assert_result("v=>p\nv=Number()\np=Print()\nv=Print()\np=Number()\n",
                  "<Number:/v Nil>\n<Print:/p >\n<Print:/v-1 >\n<Number:/p-1 Nil>\n");

  }
  
  void test_comment( void )
  { assert_result("v1=Number()    # this is a comment\n",     "<Number:/v1 Nil>\n");
    assert_result("v2=Number()    # this is a comment\n",      "<Number:/v2 Nil>\n");
    assert_result("v3=Number(4.02)#this is a comment\n# bobob\n", "<Number:/v3 4.02>\n");
  }
};