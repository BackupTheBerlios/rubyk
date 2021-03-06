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

#ifndef RUBYK_SRC_CORE_LUA_SCRIPT_H_
#define RUBYK_SRC_CORE_LUA_SCRIPT_H_

#include "oscit/script.h"
#include "node.h"

class Outlet;
struct lua_State;
typedef int (*lua_CFunction) (lua_State *L);

class LuaScript : public Node, public Script {
public:
  virtual const Value init() {
    return lua_init();
  }
  
  virtual ~LuaScript();
  
  /** Call a function in lua.
   */
  const Value call_lua(const char *function_name, const Value &val);
protected:
  /** Initialization (build methods, load libraries, etc).
   */
  const Value lua_init();
  
  /** Script compilation.
   */
  virtual const Value eval_script();
  
  /** "inlet" method in lua to create/update an inlet.
   *  @param val name of the inlet & type
   *  @return number of values on lua stack (0)
   */
  int lua_inlet(const Value &val);
  
  /** "build_outlet_" method in lua to create/update an outlet (used by rubyk library).
   *  @param val name of the outlet & type
   *  @return number of values on stack: 1 = pointer to outlet
   */
  int lua_build_outlet(const Value &val);
  
  template <class T, int (T::*Tmethod)(const Value &)>
  static int cast_method_for_lua(lua_State *L) {
    T *node = (T*)lua_this(L);
    if (node) {
      Value res = stack_to_value(L);
      if (!res.is_empty()) {
        // check signature ?
        return (node->*Tmethod)(res);
      } else {
        return 0;
      }
    } else {
      return 0;
    }
  }
  
  virtual void set_script_ok(bool state) {
    this->Script::set_script_ok(state);
    set_is_ok(state);
  }
  
private:
  static LuaScript *lua_this(lua_State *L);
  
  /** Pop all the stack as a list value.
   */
  static const Value stack_to_value(lua_State *L, int start_index = 1);
  
  /** Get the value at the given index from the lua context.
   *  If index is 0, get all the stack as a list value.
   */
  static bool value_from_lua(lua_State *L, int index, Value *res);
  
  /** Get a list of values from the table at the given index.
   *  This method assumes the object at the given index is
   *  a table.
   */
  static bool list_from_lua(lua_State *L, int index, Value *res);
  
  /** Push a value on top of lua stack.
   */
  static bool lua_pushvalue(lua_State *L, const Value &val);

  /** Push a ListValue as a table on top of lua stack.
   */
  static bool lua_pushlist(lua_State *L, const Value &val);
  
  /** "send_" method in lua (used by rubyk library in Outlet 'class').
   */
  static int lua_send(lua_State *L);
  
  /** Retrieve outlet pointer from the element at index i in the lua stack.
   */
  static bool outlet_from_lua(lua_State *L, int index, Outlet **outlet);
  
  template <class T, int (T::*Tmethod)(const Value &)>
  void register_lua_method(const char *name) {
    register_lua_method(name, &cast_method_for_lua<T, Tmethod>);
  }
  
  void register_lua_method(const char *name, lua_CFunction function);
  
  void register_custom_types();
  
  /** Printout the stack content. This is useful during debugging.
   */
  static void dump_stack(lua_State *L, const char *msg, int index);
  
  /** Every script has its own lua environment.
   */
  lua_State * lua_;
};

#endif // RUBYK_SRC_CORE_LUA_SCRIPT_H_