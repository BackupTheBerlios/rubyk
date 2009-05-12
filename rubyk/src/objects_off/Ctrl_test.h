#include "test_helper.h"

class CtrlTest : public ParseHelper
{
public:
  
  void test_send_ctrl( void ) 
  { 
    parse("n=Ctrl(channel:2)\nn=>p\n");

    // defaults
    assert_print("n.bang()\n",    "<Midi ~2:1(64), 0>\n");
    assert_print("n.bang(100)\n", "<Midi ~2:1(100), 0>\n");
    assert_print("n.bang(82)\n",  "<Midi ~2:1(82), 0>\n");
    assert_print("n.bang()\n",    "<Midi ~2:1(82), 0>\n");
  }

  void test_ctrl_slope( void ) 
  { 
    parse("n=Ctrl(slope:100)\nn=>p\n"); // 100 increments in 1 second

    // 100 [ms] minimum * 100 / 1000 => 10 steps ==> 64 + 10 = 74
    assert_print("n.bang(100)\n", "<Midi ~1:1(74), 0>\n");
    assert_print("n.move()\n",    "<Midi ~1:1(84), 0>\n");
    assert_print("n.move()\n",    "<Midi ~1:1(94), 0>\n");
    assert_print("n.move()\n",    "<Midi ~1:1(100), 0>\n");
  }

};