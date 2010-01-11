#include "lua_script.h"
#include <stdlib.h>  // exit
#include <stdio.h>
#include "opengl.h"

extern "C" {
#include <LuaGL.h>
#include <LuaGLUT.h>
}

/////////////// GLWINDOW HACK ///////////

typedef void (*plot_thread)(void * pEvent);
extern plot_thread gGLWindowStartThread;
extern void * gGLWindowNode;
extern bool   gQuitGl;  /**< Used to tell thread to exit. */
extern bool   gRunning; /**< Used to tell when server has stopped running. */

/////////////////////////////////////

class GLWindow : public LuaScript
{
public:
  virtual ~GLWindow()
  {
    ///////////// GLWINDOW HACK ////////////////
    gQuitGl = true;
    glutPostRedisplay();
    ////////////////
  }
  
  bool init(const Value &p)
  { 
    mHeight           = 600;
    mWidth            = 800;
    mNeedRedisplay    = false;
    mNeedScriptReload = false;
    mFullscreen       = false;
    lua_Init          = false;
    lua_Reshape       = false;
    lua_Draw          = false;
    lua_MouseMove     = false;
    lua_KeyPress      = false;
    mMaxFPS           = 30.0;
    mLastDraw         = 0;
    mFPS_start        = worker_->current_time_;
    mFPS_i            = 0;
    mFPS              = 0;
    
    mTitle        = "GLWindow";
    TRY(mDisplaySize, set_sizes(1, 2));
    mDisplaySizeValue.set(mDisplaySize);
    
    TRY(mMouseMatrix, set_sizes(1, 4));
    mMouseMatrix.data[3] = 1.0; // mouseUp
    mMouseMatrixValue.set(mMouseMatrix);
    
    ////////////// GLWINDOW HACK ////////////
    gGLWindowNode = (void*)this;
    gGLWindowStartThread = &GLWindow::start_opengl;
    gQuitGl = false;
    ////////////////////////////////////

    return true;
  }
  
  bool set (const Value &p)
  {
    std::string s;
    mHeight     = p.val("height", mHeight);
    mWidth      = p.val("width", mWidth);
    mFullscreen = p.val("fullscreen", mFullscreen);
    mTitle      = p.val("title", mTitle);
    mMaxFPS     = p.val("fps", mMaxFPS);
    if (mMaxFPS > 0)
      mMinFPSTime = (time_t)((real_t)ONE_SECOND / mMaxFPS);
    else
      mMinFPSTime = 0;
      
    set_lua(p);
    
    bang(gBangValue);
    
    return true;
  }
  
  // inlet 1
  virtual void bang(const Value &val)
  {
    mNeedRedisplay = true;
    send(1, sig);
  }
  
  virtual void draw(const Value &val)
  {
    if (mNeedScriptReload) {
      mMutex.lock();
        script_ok_ = eval_gl_script(script_);
      mMutex.unlock();
      
      mNeedScriptReload = false;
    }
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    compute_fps();
    
    mLastDraw = worker_->current_time_;
    
    if (lua_Draw) {
      protected_call_lua("draw",sig);
    } else {
      send(2, mDisplaySizeValue);
    }
  }
  
  virtual void key_press(unsigned char pKey, int x, int y)
  {
    Value s;
    s.set(pKey);
    if (!handle_default_keys(pKey)) {
      worker_->lock();
        if (lua_KeyPress) {
          protected_call_lua("key_press",s);
        } else {
          send(3, (int)pKey);
        }
      worker_->unlock();
    }
  }
  
  virtual void mouse_move(int x, int y)
  {
    mMouseMatrix.data[0] = (real_t)x;
    mMouseMatrix.data[1] = mDisplaySize.data[1] - (real_t)y;
    worker_->lock();
      if (lua_MouseMove) {
        protected_call_lua("mouse_move",mMouseMatrixValue);
      } else {
        send(4, mMouseMatrix);
      }
    worker_->unlock();
  }
  
  virtual void mouse_click(int button, int state, int x, int y)
  {
    mMouseMatrix.data[2] = (real_t)button;
    mMouseMatrix.data[3] = (real_t)state;
    mouse_move(x,y);
  }
  
  virtual const Value inspect(const Value &val) 
  { 
    bprint(mSpy, mSpySize,"%s %ix%i %.1ffps", mTitle.c_str(), mWidth, mHeight, mFPS);  
  }
  
protected:
  
  //////////// GLWINDOW HACK /////////////
  static void start_opengl (void * data)
  {
    GLWindow * node = (GLWindow*)data;
    pthread_setspecific(sThisKey, data);
    set_is_opengl_thread();
    node->start_loop();
    // never reaches this
  }
  ////////////////////////////////////
  
  bool handle_default_keys(unsigned char pKey)
  {
    switch(pKey) {
    case ' ':
      mFullscreen = !mFullscreen;
      update_window_size();
      return true;
    case '\e':
      worker_->lock();
        worker_->quit();
      worker_->unlock();
      return true;
    default:
      return false;
    }
  }
  
  void update_window_size()
  {
    if (mFullscreen) {
      glutFullScreen();
    } else {
      glutReshapeWindow(mWidth, mHeight);
    }
  }
  
  /** Window size has changed. */
  void reshape(int w, int h)
  {
    
    mDisplaySize.data[0] = w;
    mDisplaySize.data[1] = h;
    
    if (!mFullscreen) {
      mWidth  = w;
      mHeight = h;
    }
    
    if (lua_Reshape) {
      protected_call_lua("reshape",mDisplaySizeValue);  
    } else {
      glViewport(0, 0, w, h);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluOrtho2D(0, w, 0, h);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
    }
  }
  
  void idle()
  { 
    if (mNeedRedisplay || mMaxFPS == 0 || (worker_->current_time_ >= mLastDraw + mMinFPSTime)) {
      glutPostRedisplay();
      mNeedRedisplay = false;
    }
  }
  
  virtual void stop_my_threads()
  {
    std::list<pthread_t>::iterator it  = mThreadIds.begin();
    std::list<pthread_t>::iterator end = mThreadIds.end();

    int i = 0;
    while(it != end) {
      pthread_t id = *it;
      it = mThreadIds.erase(it); // tell thread to stop
      glutPostRedisplay();     // needed by opengl threads so they run one last time
      pthread_join(id, NULL);  // wait
      i++;
    }
  }

private:
  
  static void cast_draw (void)
  {
    GLWindow * node = (GLWindow*)thread_this();
    //////////////// GLWINDOW HACK //////////
    struct timespec sleeper;
    sleeper.tv_sec  = 0; 
    sleeper.tv_nsec = 10 * 1000000; // 1 ms
    
    if (gQuitGl) {
      while (gRunning) {
        nanosleep (&sleeper, NULL);
      }
      exit(0);
    /////////////////////////////////////
    //if (!node->run_thread()) {
      //pthread_exit(0);
    
      /* glutDestroyWindow(node->mId); */
      /* glutDestroyWindow makes rubyk die. Bad, bad, bad. */
      return;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    node->draw(node->mDisplaySizeValue);
    glutSwapBuffers ( );
  }
  
  static void cast_idle (void)
  {
    GLWindow * node = (GLWindow*)thread_this();
    node->idle();
  }
  
  static void cast_key_press (unsigned char pKey, int x, int y)
  {
    GLWindow * node = (GLWindow*)thread_this();
    node->key_press(pKey, x, y);
  }
  
  static void cast_mouse_move (int x, int y)
  {
    GLWindow * node = (GLWindow*)thread_this();
    node->mouse_move(x,y);
  }
  
  static void cast_mouse_click (int button, int state, int x, int y)
  {
    GLWindow * node = (GLWindow*)thread_this();
    node->mouse_click(button, state, x, y);
  }
  
  static void cast_reshape (int pWidth, int pHeight)
  {
    GLWindow * node = (GLWindow*)thread_this();
    node->reshape(pWidth,pHeight);
  }

  /* Init and start openGL thread. */
  void start_loop()
  {  
    // runs in new thread
    
    int argc = 0;
    const char *argv[] = {mTitle.c_str()};
    
    glutInit(&argc, const_cast<char**>(argv));

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(mWidth, mHeight);
    mId = glutCreateWindow(mTitle.c_str());
    
    glutDisplayFunc(&GLWindow::cast_draw);
    glutIdleFunc(&GLWindow::cast_idle);
    glutKeyboardFunc(&GLWindow::cast_key_press);
    glutMotionFunc(&GLWindow::cast_mouse_move);
    glutPassiveMotionFunc(&GLWindow::cast_mouse_move);
    glutMouseFunc(&GLWindow::cast_mouse_click);
    glutReshapeFunc(&GLWindow::cast_reshape);

    update_window_size();
    
    /* This thread runs with a lower priority. */
    worker_->normal_priority();
    glutMainLoop();
  }
  
  void init_gl ()
  {
    if (lua_Init) {
      protected_call_lua("init");
    } else {
      glEnable(GL_POINT_SMOOTH);
      glEnable(GL_SMOOTH);
      glEnable(GL_BLEND);                                // Enable alpha blending
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set blend function 
      
      glClearDepth(1.0);
      glDepthFunc(GL_LEQUAL);

      // glEnable(GL_CULL_FACE);
      // glEnable(GL_DEPTH_TEST);

      glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really nice perspective
      glClearColor(0.2,0.2,0.2,0.5);
    }
  }
  
  bool eval_script(const std::string &pScript) 
  {
    // this is run inside a call to lua (protected).
    // except when run from "set_lua"
    if (!is_opengl_thread()) {
      script_ = pScript;
      mNeedScriptReload = true;
      return true;
    } else {
      return eval_gl_script(pScript);
    }
  }
  
  bool eval_gl_script(const std::string &pScript)
  {
    // this is always run inside a call to lua (protected).
    script_ok_ = eval_lua_script(pScript);
  
    if (script_ok_) {
      lua_Init      = lua_has_function("init");
      lua_Reshape   = lua_has_function("reshape");
      lua_Draw      = lua_has_function("draw");
      lua_MouseMove = lua_has_function("mouse_move");
      lua_KeyPress  = lua_has_function("key_press");
    } else {
      lua_Init      = false;
      lua_Reshape   = false;
      lua_Draw      = false;
      lua_MouseMove = false;
      lua_KeyPress  = false;
    }
    // draw lock
    mMutex.unlock();
    // unlock for init lock
    init_gl();
    mMutex.lock();
    // back in draw lock
    update_window_size();
    return script_ok_;
  }
  
  inline void protected_call_lua(const char * key, const Value &val)
  {
    mMutex.lock();
      call_lua(key, sig);
    mMutex.unlock();
  }
  
  inline void protected_call_lua(const char * key)
  {
    mMutex.lock();
      call_lua(key);
    mMutex.unlock();
  }

  /* open all standard libraries and openGL libraries (called by LuaScript on init) */
  void open_lua_libs()
  {
    open_base_lua_libs();
    open_opengl_lua_libs();
  }

  /* open base lua libraries */
  void open_opengl_lua_libs()
  {
    open_lua_lib("opengl", luaopen_opengl);
    open_lua_lib("glut", luaopen_glut); 
  }
  
  void compute_fps()
  {
    mFPS_i++;
    if (worker_->current_time_ >= mFPS_start + ONE_SECOND) {
      mFPS = (real_t)mFPS_i * ONE_SECOND / (worker_->current_time_ - mFPS_start);
      mFPS_i = 0;
      mFPS_start = worker_->current_time_;
    }
  }
  
  std::string mTitle;        /**< Window title. */
  Matrix      mDisplaySize;         /**< Window size. */
  Value      mDisplaySizeValue;   /**< Wrapper around display size. */
  int         mHeight;       /**< Window height (in pixels). */
  int         mWidth;        /**< Window width (in pixels). */
  int         mId;           /**< Window id. */
  bool        mFullscreen;   /**< True if fullscreen is enabled. */
  bool        lua_Init;      /**< True if there is a Lua "init" function. */
  bool        lua_Reshape;   /**< True if there is a Lua "reshape" function. */
  bool        lua_Draw;      /**< True if there is a Lua "draw" function. */
  bool        lua_MouseMove; /**< True if there is a Lua "mouse_move" function. */
  bool        lua_KeyPress;  /**< True if there is a Lua "key_press" function. */
  pthread_t   mThread;       /**< Thread running all openGL stuff. */
  Matrix      mMouseMatrix;  /**< Mouse position matrix. */
  Value      mMouseMatrixValue; /**< Wrapper around mMouseMatrix. */
  Mutex       mMutex;
  Real      mMaxFPS;       /**< Limit number of frames per second. (0 = no limit). */
  time_t      mMinFPSTime;   /**< Minimal time interval between two draws (computed from mMaxFPS). */
  size_t      mFPS_i;        /**< Frame counter (used to compute fps). */
  Real      mFPS;          /**< Last computed number of frames per second. */
  time_t      mFPS_start;    /**< Time when the last fps counter reset was called (used to compute fps). */
  time_t      mLastDraw;     /**< Last time the draw command was called. */
  
protected:
  
  bool mNeedRedisplay;
  bool mNeedScriptReload;
};


extern "C" void init(Planet &planet) {
  CLASS(GLWindow)
  OUTLET(GLWindow, out)
  OUTLET(GLWindow, draw)
  OUTLET(GLWindow, keys)
  OUTLET(GLWindow, mousexy)
  SUPER_METHOD(GLWindow, Script, load)
  SUPER_METHOD(GLWindow, Script, script)
}