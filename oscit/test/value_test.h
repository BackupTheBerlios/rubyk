#include "test_helper.h"
#include "oscit/values.h"

class ValueTest : public TestHelper
{  
public:
  void test_create( void )
  {
    Value v;
    
    assert_true (v.is_nil());
    assert_false(v.is_real());
    assert_false(v.is_string());
    assert_false(v.is_list());
    assert_false(v.is_error());
    
    assert_equal("N", v.type_tag());
  }
  
  void test_create_with_char( void ) {
    Value v('N');
    
    assert_true(v.is_nil());
  }
  
  void test_create_with_TypeTag( void ) {
    Value v(TypeTag(""));
    Value v2(TypeTag("N"));
    
    assert_true(v.is_nil());
    assert_true(v2.is_nil());
  }
  
  void testCopy( void )
  {
    Value v;
    Value v2(v);
    Value v3(1.2);
    
    assert_true(v3.is_real());
    
    v3 = v;
    
    assert_true(v3.is_nil());
    assert_true(v2.is_nil());
  }
  
  void testSet( void )
  {
    Value v(1.2);
    
    assert_true(v.is_real());
    v.set();
    assert_true(v.is_nil());
  }
  
  void testSetTag( void )
  {
    Value v(1.2);
    
    v.set_type_tag("N");
    assert_true(v.is_nil());
  }
  
  void testSetType( void )
  {
    Value v(1.2);
    
    v.set_type(NIL_VALUE);
    assert_true(v.is_nil());
  }
};