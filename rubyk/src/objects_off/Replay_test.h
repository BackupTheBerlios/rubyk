#include "test_helper.h"

class ReplayTest : public ParseHelper
{
public:
  void test_play( void ) 
  { 
    parse("n=Replay(vector:5 file:\"test/fixtures/replay.rec\")\nn=>p\n");
    
    assert_print("n.b\n",   "<Matrix [  1.00  2.00  3.00  4.00  5.00 ], 1x5>\n");
    assert_print("n.b\n",   "<Matrix [  11.00  12.00  13.00  14.00  15.00 ], 1x5>\n");
    assert_print("n.b\n",   "<Matrix [  21.00  22.00  23.00  24.00  25.00 ], 1x5>\n");
  }
  
  void test_record( void ) 
  { 
    parse("n=Replay(file:\"test/fixtures/replay_record.rec\")\nn.record\nn=>p\n");
    
    assert_print("n.b(9,8,7)\n",   "n: recording started (vector size 3).\n<Matrix [  9.00  8.00  7.00 ], 1x3>\n");
    assert_print("n.b(6,5,4)\n",   "<Matrix [  6.00  5.00  4.00 ], 1x3>\n");
    assert_print("n.b(3,2,1)\n",   "<Matrix [  3.00  2.00  1.00 ], 1x3>\n");
    
    assert_print("n.play\n","n: starting playback (buffer 3x3).\n");
    assert_print("n.b\n",   "<Matrix [  9.00  8.00  7.00 ], 1x3>\n");
    assert_print("n.b\n",   "<Matrix [  6.00  5.00  4.00 ], 1x3>\n");
    assert_print("n.b\n",   "<Matrix [  3.00  2.00  1.00 ], 1x3>\n");
    assert_print("n.b\n",   "<Matrix [  9.00  8.00  7.00 ], 1x3>\n");
    assert_print("n.b\n",   "<Matrix [  6.00  5.00  4.00 ], 1x3>\n");
    assert_print("n.b\n",   "<Matrix [  3.00  2.00  1.00 ], 1x3>\n");
    assert_print("n.b\n",   "<Matrix [  9.00  8.00  7.00 ], 1x3>\n");
    
    
    remove("test/fixtures/replay_record.rec");
  }
  
  void test_change_size_during_recording( void ) 
  { 
    parse("n=Replay(file:\"test/fixtures/replay_record.rec\")\nn.record\nn=>p\n");
    
    assert_print("n.b(9,8)\n",     "n: recording started (vector size 2).\n<Matrix [  9.00  8.00 ], 1x2>\n");
    assert_print("n.b(7,6,5,4)\n", "<Matrix [  7.00  6.00  5.00  4.00 ], 1x4>\n");
    assert_print("n.b(3,2,1)\n",   "<Matrix [  3.00  2.00  1.00 ], 1x3>\n");
    
    assert_print("n.play\n"    ,   "n: starting playback (buffer 4x2).\n");
    assert_print("n.b\n",   "<Matrix [  9.00  8.00 ], 1x2>\n");
    assert_print("n.b\n",   "<Matrix [  7.00  6.00 ], 1x2>\n");
    assert_print("n.b\n",   "<Matrix [  5.00  4.00 ], 1x2>\n");
    assert_print("n.b\n",   "<Matrix [  3.00  2.00 ], 1x2>\n");
    assert_print("n.b\n",   "<Matrix [  9.00  8.00 ], 1x2>\n");
    
    
    remove("test/fixtures/replay_record.rec");
  }
};