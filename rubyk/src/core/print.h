/*
  ==============================================================================

   This file is part of the RUBYK project (http://rubyk.org)
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

#ifndef _PRINT_H_
#define _PRINT_H_
#include "node.h"
#include <ostream>

class Print : public Node
{
public:
  TYPED("Object.Node.Print")
  
  Print() : output_(&std::cout), prefix_("") {}
  
  const Value init() {
    if (prefix_ == "") prefix_ = name_;
    return gNilValue;
  }
  
  // [1], {2}
  const Value print(const Value &val) {
    if (val.is_nil()) {
      *output_ << prefix_ << ": Bang!\n";
    } else {
      *output_ << prefix_ << ": " << val.lazy_json() << std::endl;
    }
    return val;
  }
  
  // [2], {1}
  const Value prefix(const Value &val) {
    if (val.is_string()) {
      prefix_ = val.str();
    }
    return Value(prefix_);
  }
  
  virtual void inspect(Value *hash) const {
    hash->set("prefix", prefix_);
  }
  
  void set_output(std::ostream *output) { output_ = output; }
  
  std::ostream * output_;
private:
  std::string   prefix_;
};

#endif // _PRINT_H_