#include "lua_script.h"
#include "text_command.h"

/*** Lua script with macro capability (can execute rubyk commands). */
class Macro : public LuaScript
{
public:
  
  bool init ()
  {
    mCmd.set_output(std::cout);
    mCmd.set_input(std::cin);
    mCmd.set_server(*worker_);
    mCmd.set_silent(true);
    return true;
  }
  
  bool set (const Value &p)
  { 
    return set_lua(p);
  }
  
  
  bool eval_script(const std::string &pScript) 
  {
    if (pScript.find("function bang()") != std::string::npos) {
      script_ = pScript;
    } else {
      script_ = std::string("function bang()\nparse_command([[").append(pScript).append("\n]])\nend\n");
    }
    
    return eval_lua_script(script_);
  }

  // inlet 1
  void bang(const Value &val)
  {
    call_lua("bang", sig);
  }
  
  void in2(const Value &val)
  { set_lua_global("in2", sig); }
  
  void in3(const Value &val)
  { set_lua_global("in3", sig); }
  
  void in4(const Value &val)
  { set_lua_global("in4", sig); }
  
  void in5(const Value &val)
  { set_lua_global("in5", sig);}
  
  void in6(const Value &val)
  { set_lua_global("in6", sig);}
  
  void in7(const Value &val)
  { set_lua_global("in7", sig);}
  
  void in8(const Value &val)
  { set_lua_global("in8", sig);}
  
  void in9(const Value &val)
  { set_lua_global("in9", sig);}
  
  void in10(const Value &val)
  { set_lua_global("in10", sig);}
  
  /** Execute macro from lua. */
  int parse_command()
  {
    std::string str;
    if (string_from_lua(&str)) {
      // server is currently locked by the execution of the Lua script calling parse. We must unlock.
      worker_->unlock();
      mCmd.parse(str);
      // lock back before going back into lua.
      worker_->lock();
    }
    return 0;
  }
  
private:
  Command mCmd;        /**< Command parser. */
};

extern "C" void init(Planet &planet) {
  CLASS (Macro)
  INLET (Macro, in2)
  INLET (Macro, in3)
  INLET (Macro, in4)
  INLET (Macro, in5)
  INLET (Macro, in6)
  INLET (Macro, in7)
  INLET (Macro, in8)
  INLET (Macro, in9)
  INLET (Macro, in10)
  OUTLET(Macro, out)
  OUTLET(Macro, out2)
  OUTLET(Macro, out3)
  OUTLET(Macro, out4)
  OUTLET(Macro, out5)
  OUTLET(Macro, out6)
  OUTLET(Macro, out7)
  OUTLET(Macro, out8)
  OUTLET(Macro, out9)
  OUTLET(Macro, out10)
  METHOD_FOR_LUA(Macro, parse_command)
  SUPER_METHOD(Macro, Script, load)
  SUPER_METHOD(Macro, Script, script)
}
