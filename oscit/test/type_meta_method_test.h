/*
  ==============================================================================

   This file is part of the OSCIT library (http://rubyk.org/liboscit)
   Copyright (c) 2007-2009 by Gaspard Bucher - Buma (http://teti.ch).

  ------------------------------------------------------------------------------

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.

  ==============================================================================
*/

#include "test_helper.h"
#include "oscit/root.h"
#include "mock/dummy_object.h"

class TypeMetatMethodTest : public TestHelper
{
public:
  void test_range_type( void ) {
    Root root;
    root.adopt(new DummyObject("foo", 4.25));
    Value res;

    res = root.call(TYPE_PATH, Value(""));
    assert_equal("", res[0].str());
    res = res[1];
    assert_equal("s", res.type_tag()); // no information ...

    res = root.call(TYPE_PATH, Value("/foo"));
    assert_equal("/foo", res[0].str());
    res = res[1];
    assert_equal("fffss", res.type_tag());
    assert_equal(4.25,  res[0].r); // current
    assert_equal(0.0,   res[1].r); // min
    assert_equal(127.0, res[2].r); // max
    assert_equal("lux", res[3].str()); // unit
    assert_equal(DUMMY_OBJECT_INFO, res[4].str()); // info

    res = root.call(TYPE_PATH, Value("/blah"));
    assert_equal("/blah", res[0].str());
    res = res[1];
    assert_true(res.is_error());
    assert_equal(NOT_FOUND_ERROR, res.error_code());
  }

  void test_select_type( void ) {
    Root root;
    root.adopt(new DummyObject2("foo", "yuv"));
    Value res;

    res = root.call(TYPE_PATH, Value("/foo"));
    assert_equal("/foo", res[0].str());
    res = res[1];
    assert_equal("ssss", res.type_tag());
    assert_equal("yuv",  res[0].str()); // current
    assert_equal("rgb,rgba,yuv",  res[1].str()); // current
    assert_equal("color mode", res[2].str()); // unit
    assert_equal("Set color mode.", res[3].str()); // info
  }

  void test_any_type( void ) {
    Root root;
    root.adopt(new DummyObject("foo", 1.23, AnyIO("This is the info string.")));
    Value res;
    res = root.call(TYPE_PATH, Value("/foo"));
    assert_equal("/foo", res[0].str());
    res = res[1];
    assert_equal("*s", res.type_tag());
    assert_true(res[0].is_any());
    assert_equal("This is the info string.", res[1].str()); // info
  }

  void test_list_type( void ) {
    Root root;
    root.adopt(new DummyObject("Haddock", Value("Haddock").push_back(42), Value(Json("[[\"\", 0.0], \"name\", \"years old\", \"Set captain with name and age.\"]"))));
    Value res;
    res = root.call(TYPE_PATH, Value("/Haddock"));
    assert_equal("/Haddock", res[0].str());
    res = res[1];
    assert_equal("[sf]sss", res.type_tag());
    assert_equal("[\"Haddock\", 42]", res[0].to_json()); // info
  }

  void test_hash_type( void ) {
    Root root;
    root.adopt(new DummyObject("dog_food", Value(Json("{lazy:\"dog\", silly:\"cats and mices\"}")), HashIO("Blah blah.")));
    Value res;
    res = root.call(TYPE_PATH, Value("/dog_food"));
    assert_equal("/dog_food", res[0].str());
    res = res[1];
    assert_equal("Hs", res.type_tag());
    assert_equal("{\"lazy\":\"dog\", \"silly\":\"cats and mices\"}", res[0].to_json()); // hash
    assert_equal("Blah blah.", res[1].str()); // info
  }

  void test_matrix_type( void ) {
    Root root;
    root.adopt(new DummyObject("master_of_time", MatrixValue(1,5), MatrixIO(1,5,"Stupid matrix.")));
    Value res;
    res = root.call(TYPE_PATH, Value("/master_of_time"));
    assert_equal("/master_of_time", res[0].str());
    res = res[1];
    assert_true(res.is_list());
    assert_equal("Ms", res.type_tag());
    assert_equal("\"Matrix 1x5\"", res[0].to_json()); // matrix to json ...
    assert_equal("Stupid matrix.", res[1].str()); // info
  }

  void test_no_input( void ) {
    Root root;
    root.adopt(new Object("foo"));
    Value res;

    res = root.call(TYPE_PATH, Value("/foo"));
    assert_equal("/foo", res[0].str());
    res = res[1];
    assert_equal("No information on this node.", res.str());
  }

  void test_type_with_nil( void ) {
    Root root(NoIO("This is the root node."));
    Value res;

    res = root.call(TYPE_PATH, gNilValue);
    assert_true(res.is_nil());
  }
};