#include "command.h"
#include "rubyk.h"
#include "class.h"
#include "mutex.h"

#undef DEBUG_PARSER

%%{
  machine command;
  write data noerror;
}%%

void Command::initialize()
{
  int cs;
  
  mAction = NO_ACTION;
  mServer = NULL;
  mQuit   = false;
  mTokenIndex = 0;
  mSilent = false;
  
  %% write init;
  mCurrentState = cs;
}

int Command::do_listen()
{
  char iss[1024];
  if (!mSilent)
    *mOutput << "Welcome to rubyk !\n\n";
  clear();
  prompt();

  while(!mQuit && !mInput->eof()) {
    mInput->getline(iss,1023); // '\n'
    parse(iss);
    parse("\n");
  }
  return 0; // thread return value
}

void Command::parse(const std::string& pStr)
{
  const char *p  = pStr.data(); // data pointer
  const char *pe = p + pStr.size(); // past end
  int cs = mCurrentState;        // restore machine state
  

  %%{
    action a {
      if (mTokenIndex >= MAX_TOKEN_SIZE) {
        std::cerr << "Buffer overflow !" << std::endl;
        // stop parsing
        return;
      }
#ifdef DEBUG_PARSER
      printf("_%c_",fc);
#endif
      mToken[mTokenIndex] = fc; /* append */
      mTokenIndex++;     
    }
    
    action set_var { set_from_token(mVariable);}
    
    action set_method { set_from_token(mMethod);}
    
    action set_key { set_from_token(mKey);}
  
    action set_class { set_class_from_token();}
  
    action set_value { set_from_token(mValue);}
    
    action set_from_port {
      set_from_token(mValue);
      mFromPort = atoi(mValue.c_str());
      mFrom = mVariable;
    }
    
    action set_to_port {
      set_from_token(mTo);
      mToPort = atoi(mValue.c_str());
    }
    
    action set_single_param { set_single_param_from_token(); }
    
    action add_param { set_parameter(mKey, mValue); }
    
    action create_link {
      mToPort = atoi(mValue.c_str());
      mTo   = mVariable;
      create_link();
    }

    action create_instance { create_instance(); }
    
    action execute_method { execute_method(); }
    
    action execute_class_method { execute_class_method(); }
    
    action execute_command { execute_command(); }
    
    action debug { printf("[%c]", p[0]); }
    
    action prompt {
      if (!mQuit) {
        clear();
        prompt();
      }
    }
    action error {
      fhold; // move back one char
      *mOutput << "Syntax error !" << std::endl;
      clear();
      prompt();
      fgoto eat_line; // eat the rest of the line and continue parsing
    }
  
    eat_line := [^\n]* '\n' @{ fgoto main; };
    
    ws     = (' ' | '\t');
  
    identifier = (lower (alnum | '_')*) $a;
  
    var    = identifier %set_var;
    
    method = identifier %set_method;
  
    class  = (upper (alnum | '_')*) $a %set_class;
  
    string  = '"' ([^"\\] | '\n' | ( '\\' (any | '\n') ))* $a '"';
    float   = [\-+]? $a ('1'..'9' digit* '.' digit+) $a;
    integer = [\-+]? $a ('1'..'9' digit*) $a;
  
    value  = (string | float | integer ) %set_value ;
  
    key    = identifier %set_key;
  
    param  = (key ':' ws* value) %add_param;
  
    parameters = value %set_single_param | (param ws*)+;
  
    create_instance = var ws* '=' ws* class '(' parameters? ')' ;
  
    create_link = var '.' integer %set_from_port ws* '=>' ws* integer %set_value '.' var %set_to_port;
    
    execute_method       = var   '.' method ( '(' parameters? ')' )? ;
    
    execute_class_method = class '.' method ( '(' parameters? ')' )? ;

    execute_command = method ( '(' parameters? ')' )?;
  
    main := ((execute_command %execute_command
            | execute_method  %execute_method
            | execute_class_method  %execute_class_method
            | create_instance %create_instance 
            | create_link %create_link 
            | ws* )  '\n' )+ @prompt $err(error);
    write exec;
  }%%
//  printf("{%s}\n",p);
  mCurrentState = cs;
}


void Command::close()
{
  mQuit = true;
  pthread_join( mThread, NULL); // wait for child to finish
}

void Command::set_from_token (std::string& pElem)
{
  mToken[mTokenIndex] = '\0';
#ifdef DEBUG_PARSER
  if (&pElem == &mValue) std::cout <<    "[val " << mToken << "]" << std::endl;
  if (&pElem == &mVariable) std::cout << "[var " << mToken << "]" << std::endl;
  if (&pElem == &mClass) std::cout <<    "[cla " << mToken << "]" << std::endl;
#endif
  pElem = mToken;
  mTokenIndex = 0;
}


void Command::set_class_from_token  () 
{
  set_from_token(mClass);
  // FIXME: this might not be needed if class is set before param
  if (mSingleParam != "") {
    std::string key = mClass;
    std::transform(key.begin(), key.end(), key.begin(), tolower);
    mParameters.set(key, mSingleParam);
    mSingleParam = "";
  }
}

void Command::set_single_param_from_token () 
{
  mSingleParam = mValue;
  std::string key;
  if (mClass != "") {
    key = mClass;
  } else if (mMethod != "") {
    key = mMethod;
  } else {
    key = "value";
  }
  std::transform(key.begin(), key.end(), key.begin(), tolower);
  mParameters.set(key, mSingleParam);
}

void Command::set_parameter  (const std::string& pKey, const std::string& pValue) 
{
  mParameters.set(pKey,pValue);
}

// FIXME: create_instance should run in server space with concurrency locks.
void Command::create_instance()
{
  mServer->lock();
  Node * node = mServer->create_instance(mVariable, mClass, mParameters, mOutput);
#ifdef DEBUG_PARSER
  std::cout << "NEW("<< mVariable << ", " << mClass << ", " << mParameters << ")";
#endif
  if (node) {
    if (!mSilent)
      *mOutput << node->inspect() << std::endl;
  } else {
    *mOutput << "Error" << std::endl;      
  }
  mServer->unlock();
}


void Command::create_link()
{ 
  mServer->lock();
  mServer->create_link(mFrom, mFromPort, mToPort, mTo);
  mServer->unlock();

  if (!mSilent)
    *mOutput << "LINK " << mFrom << "." << mFromPort << "=>" << mToPort << "." << mTo << std::endl;
}

// FIXME: execute_method should run in server space with concurrency locks.
void Command::execute_method()
{
  Node * node;
  mServer->lock();
  if (mServer->get_instance(&node, mVariable)) {
    node->execute_method(mMethod, mParameters); 
    if (!mSilent && mMethod == "bang") *mOutput << node->inspect() << std::endl;
  } else {
    *mOutput << "Unknown node '" << mVariable << "'" << std::endl;
  }
  mServer->unlock();
}

void Command::execute_class_method()
{
  Class * klass;
  mServer->lock();
  if (Class::get(&klass, mClass)) {
    klass->execute_method(mMethod, mParameters, mOutput);
  } else {
    *mOutput << "Unknown class '" << mClass << "'" << std::endl;
  }
  mServer->unlock();
}

void Command::execute_command()
{
  Node * node;
  mServer->lock();
  if (mServer->get_instance(&node, mMethod)) {
    // inspect
    *mOutput << node->inspect() << std::endl;
  } else if (mMethod == "quit") {
    mServer->quit();
    mQuit = true;
    *mOutput << "Bye..." << std::endl;
  } else {
    *mOutput << "Unknown command '" << mMethod << "'" << std::endl;
  }
  mServer->unlock();
}

void Command::print(void)
{
  switch(mAction) {
  case CREATE_INSTANCE:
    *mOutput << mVariable << " = " << mClass << "(" << mParameters << ")" << std::endl;
    break;
  case CREATE_LINK:
    *mOutput << "[todo]" << std::endl;
    break;
  }
}

void Command::clear() 
{
  mAction     = NO_ACTION;
  mVariable   = "";
  mClass      = "";
  mSingleParam = "";
  mParameters.clear();
}
