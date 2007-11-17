#include "test_helper.h"


class TuringTest : public CxxTest::TestSuite, public ParseTest
{
public:
  /*
    a  x  -> b { X }
    a  'y'-> b { Y }
    b  ----> b {   }
    
    goto ======
       . x y
    a  0 1 1
    b  1 - -

    send ======
       . x y
    a  / X Y
    b  / - -
  */
  
  void test_compile( void ) 
  { assert_result("n=Turing(\"a  x  -> b { X }\na  'y'-> b { Y }\nb  ----> b {   }\")\nn.tables\n",
                  "#<Turing:n -->\ngoto\n 0 1 1\n 1 - -\nsend\n / 88 89\n / - -\n"); }
  void test_compile_numbers( void ) 
  { assert_result("n=Turing(\"a  x  -> b { 23 }\na  'y'-> b { 22 }\nb  ----> b {   }\")\nn.tables\n",
                  "#<Turing:n -->\ngoto\n 0 1 1\n 1 - -\nsend\n / 23 22\n / - -\n"); }
  /*
    a  x  -> b { X }
    a  'y'-> b { Y }
    b  ----> b {   }
    
    goto ======
       . x y
    a  0 1 1
    b  1 - -

    send ======
       . x y
    a  / X Y
    b  / - -
  */
  
  void test_comment( void ) 
  { assert_result("n=Turing(\"a  x  -> b { X } # this is a comment\n=begin\nmulti\nline\ncomment\n=end\na  'y'-> b { Y }\nb  ----> b {   }\")\nn.tables\n",
                  "#<Turing:n -->\ngoto\n 0 1 1\n 1 - -\nsend\n / 88 89\n / - -\n"); }
  /*    
    'x' = 120
    'y' = 121

    'X' = 88
    'Y' = 89

    test sequence is:
    x => b (X)
    ? => a (A)
    y => b (Y)
  */
  
  void test_sequence( void )
  {
    setup_with_print("n=Turing(\"a  x  -> b { X } # this is a comment\n=begin\nmulti\nline\ncomment\n=end\na  'y'-> b { Y }\nb  ----> a { A }\nb  -x--> b {   }\")\n");
    assert_bang("n.input(120)\n",  "88\n");
    assert_bang("n.input(5644)\n", "65\n");
    assert_bang("n.input(121)\n",  "89\n");
  }
  
};