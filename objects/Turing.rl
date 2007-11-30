#include "script.h"
#define MAX_NAME_SIZE 200
//#define DEBUG_PARSER

%%{
  machine turing;
  write data noerror;
}%%

class Turing : public Script
{
public:
  Turing() : mStateByName(30), mTokenNameByValue(30), mTokenByName(30) {}
  
  bool init (const Params& p)
  {
    mToken = 0;
    mState = 0;
    return init_script(p);
  }

  // inlet 1
  void bang(const Signal& sig)
  { 
    int i;
    int state;
    if (sig.get(&i)) {
      mRealToken = i;
      mToken = mTokenTable[ i % 256 ]; // translate token in the current machine values.
    }
    
    reload_script();
    if (mScriptDead) return;
    
    //std::cout << "goto\n";
    //print(std::cout, mGotoTable);
    //std::cout << "send\n";
    //print(std::cout, mSendTable);
    //
    //std::cout << "token ["<< mToken << "]" << std::endl;
    //std::cout << "state ["<< mState << "]" << std::endl;
    
    if (mDebug) *mOutput << "{" << mState << "} -" << mRealToken << "->";
      
    if ((mSend = mSendTable[mState][mToken]) != -1)
      ; // ok custom value
    else
      mSend = mSendTable[mState][0]; // use default

    if ((state = mGotoTable[mState][mToken]) != -1)
      mState = state;
    else
      mState = mGotoTable[mState][0]; // use default
    
    if (mDebug) *mOutput << "{" << mState << "}" << std::endl;
    
    /* Send the value out. */
    if (mSend == -2)
      ; // send nothing
    else
      send(mSend);
  }


  void eval_script(const std::string& pScript) 
  {
    mScript = pScript;
    int cs;
    const char * p  = mScript.data(); // data pointer
    const char * pe = p + mScript.size(); // past end
    char name[MAX_NAME_SIZE + 1];
    int  name_index = 0;
    
    int token_id = 0;
    int tok;
    int send = -2;
    
    // source state, target state
    std::string source;
    std::string target;
    int source_state = 0;
    int target_state = 0;
    
    // integrated lua script
    const char * begin_lua_script = NULL;
    std::string lua_script;
    
    
    // function call id, params
    // 1. during parse
    // 1.1 'send' ? store 0
    // 1.2 store i++ and push_back vector of method names (method_names)
    // 1.3 store arguments as vector
    // 2. during resolution
    // 2.1 foreach i, get method_names[i], push back in method_ids
    // 2.2 foreach 'send' in table, replace id by method_ids[id] (keep arguments)
    
    // a call = push args on stack, call method_id
    
    // get token values by identifier
    mTokenByName.clear();
    mTokenNameByValue.clear();
    
    std::string identifier;
    
    // to add new tokens
    std::vector< std::vector<int> >::iterator it,end;
    
    // init token table
    memset(mTokenTable, 0, sizeof(mTokenTable));
    
    mStateCount = 0;
    mTokenCount = 0;
    mGotoTable.clear();
    mSendTable.clear();
    mStateByName.clear();
    mStateNames.clear();
    
    %% write init;
    
  %%{
    action a {
      if (name_index >= MAX_NAME_SIZE) {
        *mOutput << "Name buffer overflow !\n";
        mScriptDead = true;
        return;
      }
      #ifdef DEBUG_PARSER
        printf("_%c_",fc);
      #endif
      name[name_index] = fc; /* append */
      name_index++;     
    }
    
    action set_identifier {
      name[name_index] = '\0';
      identifier = name;
      name_index = 0;
      #ifdef DEBUG_PARSER
        std::cout <<    "[identifier " << identifier << "]" << std::endl;
      #endif
    }
    
    action set_send {
      name[name_index] = '\0';
      name_index = 0;
      #ifdef DEBUG_PARSER
        std::cout <<    "[send " << name << "]" << std::endl;
      #endif
      send = (int)name[0]; //FIXME !
    }
    
    action set_source {
      source = identifier;
      #ifdef DEBUG_PARSER
        std::cout <<    "[source " << source << "]" << std::endl;
      #endif 
    }
    
    action set_target { 
      target = identifier;
      #ifdef DEBUG_PARSER
        std::cout <<    "[target " << target << "]" << std::endl;
      #endif
      source_state = get_state_id(source); // we postponed this to here to be sure state is not confused with token identifier
      target_state = get_state_id(target);
      source = target; // the last target becomes the next source
    }

    action set_token_from_identifier { 
      if(name_index) {
        // identifier: resolve to value
        name[name_index] = '\0';
        name_index = 0;
        if (!mTokenByName.get(&tok, std::string(name))) {
          *mOutput << "Syntax error. Unknown token '" << name << "' (missing declaration)\n";
          mScriptDead = true;
          return;
        }
      } else {
        *mOutput << "Syntax error: no identifier set.\n";
        mScriptDead = true;
        return;
      }
    }
    
    action set_tok_value { 
      name[name_index] = '\0';
      name_index = 0;
      #ifdef DEBUG_PARSER
        std::cout <<    "[num " << name << "]" << std::endl;
      #endif
      tok = atoi(name);
    }
    
    
    action define_token {
      mTokenByName.set(identifier, tok);
      mTokenNameByValue.set(tok, identifier);
      #ifdef DEBUG_PARSER
        std::cout << identifier << " = " << tok << std::endl;
      #endif
    }
    
    action set_token {
      // do we know this token ?
      if (!mTokenTable[tok % 256]) {
        // new token
        #ifdef DEBUG_PARSER
        printf("new token %i: %i\n", tok, mTokenCount);
        #endif
        
        mTokenTable[tok % 256] = mTokenCount + 1;
        mTokenList.push_back(tok);
        mTokenCount++;
        
        // enlarge lookup tables (add new column)
        end = mGotoTable.end();
        int counter = 0;
        for (it = mGotoTable.begin(); it < end; it++) {
          // enlarge all arrays in the table
          if (mTokenCount == 1)
            (*it).push_back(counter); // first value is counter (stay)
          else
            (*it).push_back(-1); // -1 means use default

          counter++;
        }
        
        end = mSendTable.end();
        for (it = mSendTable.begin(); it < end; it++) {
          // enlarge all arrays in the table
          if (mTokenCount == 1)
            (*it).push_back(-2); // -2 means do not send (default send in first column)
          else
            (*it).push_back(-1); // -1 means use default 
        }
        
      }
      token_id = mTokenTable[tok % 256];
    }
    
    action add_entry {
      // write the entry
      #ifdef DEBUG_PARSER
      printf("define %i - %i:%i -> %i\n", source_state, token_id, send, target_state);
      #endif
      
      mGotoTable[source_state][token_id] = target_state;
      mSendTable[source_state][token_id] = send;
      token_id = 0;
      send     = -2;
      source_state = 0;
      target_state = 0;
    }
    
    
    action error {
      fhold; // move back one char
      char error_buffer[10];
      snprintf(error_buffer, 9, "%s", p);
      *mOutput << "Syntax error near '" << error_buffer << "'." << std::endl;
      mScriptDead = true;
      return;
    }
    
    action debug {
      printf("[%c]", fc);
      fflush(stdout);
    }
    
    action begin_comment { fgoto doc_comment; }
    action end_comment   { fgoto main; }
    
    action begin_lua     { 
      std::cout << "begin_lua\n";
      begin_lua_script = p;
      fgoto lua_script; 
    }
    action end_lua       {
      lua_script.append( begin_lua_script, p - begin_lua_script - 4 );
      begin_lua_script = NULL;
      fgoto main; 
    }
    

    
    doc_comment := (('=end' %end_comment | [^\n]*) '\n')+;
    
    lua_script  := (('=end' %end_lua     | [^\n]*) '\n')+;
    
    ws     = (' ' | '\t');
    
    identifier = (alpha alnum*) $a %set_identifier;
    
    tok    = ( identifier @set_token_from_identifier | digit+ $a %set_tok_value ); # fixme: we should use 'whatever' or a-zA-Z or number
    
    send   = alnum $a %set_send;

    transition = ( '-'+ '>' | '-'* ws* tok %set_token (':' send)?  ws* '-'+ '>');

    comment = '#' [^\n]* ;
    
    begin_comment = '=begin\n' @begin_comment;
    
    sub_entry = ws+ transition ws+ identifier %set_target;
    
    entry     = identifier %set_source sub_entry;
    
    begin_lua = '=begin' ' '+ 'lua\n' @begin_lua;
    
    define_token = identifier ws* '=' ws* digit+ $a %set_tok_value;

    main  := ( ws* (entry %add_entry (sub_entry %add_entry)* (ws* comment)? | define_token %define_token | comment | begin_comment | begin_lua | ws* )  '\n' )+ $err(error);
    write exec;
    write eof;
  }%%
  
    if (begin_lua_script) {
      lua_script.append( begin_lua_script, p - begin_lua_script );
    }
    mScriptDead = false; // ok, we can receive and process signals (again).
  }

  /** Output transition and action tables. */
  void tables()
  {  
    *mOutput << "tokens\n";
    for(int i=0;i<mTokenCount;i++) {
      int tok_value = mTokenList[i];
      std::string identifier;
      if (mTokenNameByValue.get(&identifier, tok_value)) {
        *mOutput << " " << i << " : " << identifier << " = " << tok_value << "\n";
      } else {
        *mOutput << " " << i << " : " << tok_value << "\n";
      }
    }
    print_table(*mOutput, "goto", mGotoTable);
    print_table(*mOutput, "send", mSendTable);
  }
  
  /** Output tables in digraph format to produce graphs with graphviz. */
  void dot()
  {
    make_dot_graph(*mOutput);
  }

private:
  
  int get_state_id(const std::string& pName)
  {
    int state_id;
    // do we know this name ?
    if (!mStateByName.get(&state_id, pName)) {
      // new state
      state_id = mStateCount;
      mStateByName.set(pName, state_id);
      mStateNames.push_back(pName);
      // add a new line to the lookup tables
      mGotoTable.push_back( std::vector<int>(mTokenCount+1, -1) ); // -1 means use default
      mGotoTable[mStateCount][0] = mStateCount; // default: stay
      
      mSendTable.push_back( std::vector<int>(mTokenCount+1, -1) ); // -1 means use default
      mSendTable[mStateCount][0] = -2; // -2 means send 'nil'
      
      mStateCount++;
    }
    return state_id;
  }

  void print_table(std::ostream& pOutput, const char * pTitle, std::vector< std::vector<int> >& pTable) {  
    std::vector< std::vector<int> >::iterator it,end;
    end = pTable.end();
    
    // print tokens
    bprint(mBuf, mBufSize, "\n%- 8s  -", pTitle);
    pOutput << mBuf;
    for(int i=0;i<mTokenCount;i++) {
      int tok_value = mTokenList[i];
      std::string identifier;
      if (mTokenNameByValue.get(&identifier, tok_value))
        bprint(mBuf, mBufSize, " % 3s", identifier.c_str());
      else
        bprint(mBuf, mBufSize, " % 3i", tok_value);
      pOutput << mBuf;
    }
    pOutput << "\n";
    
    int state_count = 0;
    for (it = pTable.begin(); it < end; it++) {
      std::vector<int>::iterator it2,end2;
      end2 = (*it).end();
      
      bprint(mBuf, mBufSize, " % 3s : ", mStateNames[state_count].c_str());
      pOutput << mBuf;
      for ( it2 = (*it).begin(); it2 < end2; it2++ ) {
        if (*it2 == -1)
          pOutput << "   -";  // default
        else if (*it2 == -2)
          pOutput << "   /";  // do not send
        else {
          bprint(mBuf, mBufSize, " % 3i", *it2);
          pOutput << mBuf;
        }
      }
      pOutput << "\n";
      state_count++;
    } 
  }
  
  void make_dot_graph(std::ostream& out)
  {
    std::string source,target,token,send;
    
    out << "digraph " << mName << "{\n";
    out << "  rankdir=LR;\n";
  	// first node
    out << "  node [ fixedsize = true, height = 0.65, shape = doublecircle ];\n";
    out << "  " << mStateNames[0] << ";\n";
    // all other nodes
    out << "  node [ shape = circle ];\n";
    // transitions
    
    for (int i=0; i < mStateCount; i++) {
      source = mStateNames[i];
      // print default action
      token = '-';
      target = mStateNames[mGotoTable[i][0]];
      if (mSendTable[i][0] == -2) 
        send = ""; // send nothing
      else {
        send = ":";
        bprint(mBuf, mBufSize, "%i", mSendTable[i][0]);
        send.append(mBuf);
      }
      out << "  " << source << " -> " << target << " [ label = \"" << token << send << "\"];\n";
      
      // print other transitions
      for (int j=0; j < mTokenCount; j++) {
        if (!mTokenNameByValue.get(&token, mTokenList[j])) {
          bprint(mBuf, mBufSize, "%i", mTokenList[j]); // no token name
          token = mBuf;
        }
        if (mGotoTable[i][j+1] == -1)
          ;  // default, do not print
        else {
          target  = mStateNames[mGotoTable[i][j+1]];
          // source -> target [ label = "token:send" ];
          if (mSendTable[i][j+1] == -2) 
            send = ""; // send nothing
          else {
            send = ":";
            bprint(mBuf, mBufSize, "%i", mSendTable[i][j+1]);
            send.append(mBuf);
          }
          out << "  " << source << " -> " << target << " [ label = \"" << token << send << "\"];\n";
        }
      }
    }
    out << "}\n";
  }
  
  
  int  mToken;           /**< Current token value (translated). */
  int  mRealToken;       /**< Current token value (not translated). */
  int  mSend;            /**< Send result. */
  int  mState;           /**< Current state. */
  
  int  mTokenTable[256]; /**< Translate token values into their internal representation. */
  int  mStateCount;      /**< Number of states in the machine. */
  int  mTokenCount;      /**< Number of tokens recognized by the machine. */
  
  Hash<std::string, int>   mTokenByName;   /**< Dictionary returning token id from its identifier (used to  plot/debug). */
  Hash<uint, std::string>  mTokenNameByValue; /**< Dictionary returning token name from its value (used to plot/debug). */
  std::vector<int>         mTokenList;     /**< List of token values (used to plot/debug). */
  
  Hash<std::string, int>   mStateByName;   /**< Dictionary returning state id from its identifier. */
  std::vector<std::string> mStateNames;    /**< List of state names (used to plot/debug). */
  
  std::vector< std::vector<int> > mGotoTable; /**< State transition table. */
  std::vector< std::vector<int> > mSendTable; /**< State transition table. */
  
};

extern "C" void init()
{
  CLASS (Turing)
  OUTLET(Turing, output)
  METHOD(Turing, tables)
  METHOD(Turing, dot)
  SUPER_METHOD(Turing, Script, set)
  SUPER_METHOD(Turing, Script, load)
  SUPER_METHOD(Turing, Script, script)
}
