// ordered_list_test.h 
#include <cxxtest/TestSuite.h>
#include "test_helper.h"


class AliasNodeTest : public NodeTestHelper
{
public:
  void test_alias_of_method( void )
  {
    create("/grp", "Group",  ""            , "[4] /grp");
    create("/grp/one", "Number", "value: 1", "[10] /grp/one");
    create("/grp/ali", "Alias",  "original: /grp/one/value", "[16] /grp/ali");
    
    assert_call("/grp/one/value", "", "[6] 1.00");
    assert_call("/grp/ali", "3.45",   "[6] 3.45"); // /grp/ali ---> /grp/one/value
    assert_call("/grp/one/value", "", "[6] 3.45");
  }
  
  void test_alias_of_node( void )
  {
    create("/grp", "Group",  ""            , "[4] /grp");
    create("/grp/one", "Number", "value: 1", "[10] /grp/one");
    create("/grp/ali", "Alias",  "original: /grp/one", "[16] /grp/ali");

    // FIXME: what should we do here ?
    // assert_call("/grp/ali", "",   "value,in/,out/"); // /grp/ali ---> /grp/one
    // assert_call("/grp/ali/value", "",   "[8] 1.00"); // /grp/ali/value ---> /grp/one/value ?
  }
};