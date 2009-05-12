#include "test_helper.h"

class MacroTest : public ParseHelper
{
public:

  void test_parse_command( void ) 
  { 
    parse("n=Macro(\"v=Value()\nv=>p\nv.bang(3)\")\nn=>p\n");
    assert_print("n.bang\n","3.00\n");
  }
  
  /*
  state = state or 'left'
  function bang()
    if (state == 'left') then
      parse_command('left // p\nright => p\n')
      state = 'right'
    else
      parse_command('right // p\nleft => p\n')
      state = 'left'
    end
  end
  */
  void test_switch( void ) 
  { 
    parse("n=Macro(\"state = state or 'left'\nfunction bang()\nif (state == 'left') then\nparse_command('left // p\\nright => p\\n')\nstate = 'right'\nelse\nparse_command('right // p\\nleft => p\\n')\nstate='left'\nend\nend\")\nleft=Value(1)\nright=Value(2)\nleft=>p\nn=>p\n");
    assert_print("left.b\n", "1.00\n");
    assert_print("right.b\n",""      );
    // switch
    assert_print("n.bang\n", ""      );
    assert_print("left.b\n", ""      );
    assert_print("right.b\n","2.00\n");
    // switch
    assert_print("n.bang\n", ""      );
    assert_print("left.b\n", "1.00\n");
    assert_print("right.b\n","");
  }
};