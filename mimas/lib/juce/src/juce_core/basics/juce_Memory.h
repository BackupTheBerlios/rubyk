/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#ifndef __JUCE_MEMORY_JUCEHEADER__
#define __JUCE_MEMORY_JUCEHEADER__

//==============================================================================
/*
    This file defines the various juce_malloc(), juce_free() macros that should be used in
    preference to the standard calls.
*/

#if defined (JUCE_DEBUG) && JUCE_MSVC && JUCE_CHECK_MEMORY_LEAKS
  #ifndef JUCE_DLL
    //==============================================================================
    // Win32 debug non-DLL versions..

    /** This should be used instead of calling malloc directly. */
    #define juce_malloc(numBytes)                 _malloc_dbg  (numBytes, _NORMAL_BLOCK, __FILE__, __LINE__)
    /** This should be used instead of calling calloc directly. */
    #define juce_calloc(numBytes)                 _calloc_dbg  (1, numBytes, _NORMAL_BLOCK, __FILE__, __LINE__)
    /** This should be used instead of calling realloc directly. */
    #define juce_realloc(location, numBytes)      _realloc_dbg (location, numBytes, _NORMAL_BLOCK, __FILE__, __LINE__)
    /** This should be used instead of calling free directly. */
    #define juce_free(location)                   _free_dbg    (location, _NORMAL_BLOCK)

  #else
    //==============================================================================
    // Win32 debug DLL versions..

    // For the DLL, we'll define some functions in the DLL that will be used for allocation - that
    // way all juce calls in the DLL and in the host API will all use the same allocator.
    extern JUCE_API void* juce_DebugMalloc (const int size, const char* file, const int line);
    extern JUCE_API void* juce_DebugCalloc (const int size, const char* file, const int line);
    extern JUCE_API void* juce_DebugRealloc (void* const block, const int size, const char* file, const int line);
    extern JUCE_API void juce_DebugFree (void* const block);

    /** This should be used instead of calling malloc directly. */
    #define juce_malloc(numBytes)                 juce::juce_DebugMalloc (numBytes, __FILE__, __LINE__)
    /** This should be used instead of calling calloc directly. */
    #define juce_calloc(numBytes)                 juce::juce_DebugCalloc (numBytes, __FILE__, __LINE__)
    /** This should be used instead of calling realloc directly. */
    #define juce_realloc(location, numBytes)      juce::juce_DebugRealloc (location, numBytes, __FILE__, __LINE__)
    /** This should be used instead of calling free directly. */
    #define juce_free(location)                   juce::juce_DebugFree (location)
  #endif

  #if ! defined (_AFXDLL)
    /** This macro can be added to classes to add extra debugging information to the memory
        allocated for them, so you can see the type of objects involved when there's a dump
        of leaked objects at program shutdown. (Only works on win32 at the moment).
    */
    #define juce_UseDebuggingNewOperator \
      static void* operator new (size_t sz)           { void* const p = juce_malloc ((int) sz); return (p != 0) ? p : ::operator new (sz); } \
      static void* operator new (size_t sz, void* p)  { return ::operator new (sz, p); } \
      static void operator delete (void* p)           { juce_free (p); }
  #endif

#elif defined (JUCE_DLL)
  //==============================================================================
  // Win32 DLL (release) versions..

  // For the DLL, we'll define some functions in the DLL that will be used for allocation - that
  // way all juce calls in the DLL and in the host API will all use the same allocator.
  extern JUCE_API void* juce_Malloc (const int size);
  extern JUCE_API void* juce_Calloc (const int size);
  extern JUCE_API void* juce_Realloc (void* const block, const int size);
  extern JUCE_API void juce_Free (void* const block);

  /** This should be used instead of calling malloc directly. */
  #define juce_malloc(numBytes)                 juce::juce_Malloc (numBytes)
  /** This should be used instead of calling calloc directly. */
  #define juce_calloc(numBytes)                 juce::juce_Calloc (numBytes)
  /** This should be used instead of calling realloc directly. */
  #define juce_realloc(location, numBytes)      juce::juce_Realloc (location, numBytes)
  /** This should be used instead of calling free directly. */
  #define juce_free(location)                   juce::juce_Free (location)

  #define juce_UseDebuggingNewOperator \
    static void* operator new (size_t sz)           { void* const p = juce_malloc ((int) sz); return (p != 0) ? p : ::operator new (sz); } \
    static void* operator new (size_t sz, void* p)  { return ::operator new (sz, p); } \
    static void operator delete (void* p)           { juce_free (p); }

#else

  //==============================================================================
  // Mac, Linux and Win32 (release) versions..

  /** This should be used instead of calling malloc directly. */
  #define juce_malloc(numBytes)                 malloc (numBytes)
  /** This should be used instead of calling calloc directly. */
  #define juce_calloc(numBytes)                 calloc (1, numBytes)
  /** This should be used instead of calling realloc directly. */
  #define juce_realloc(location, numBytes)      realloc (location, numBytes)
  /** This should be used instead of calling free directly. */
  #define juce_free(location)                   free (location)

#endif

//==============================================================================
/** This macro can be added to classes to add extra debugging information to the memory
    allocated for them, so you can see the type of objects involved when there's a dump
    of leaked objects at program shutdown. (Only works on win32 at the moment).

    Note that if you create a class that inherits from a class that uses this macro,
    your class must also use the macro, otherwise you'll probably get compile errors
    because of ambiguous new operators.

    Most of the JUCE classes use it, so see these for examples of where it should go.
*/
#ifndef juce_UseDebuggingNewOperator
  #define juce_UseDebuggingNewOperator
#endif

//==============================================================================
#if JUCE_MSVC
  /** This is a compiler-indenpendent way of declaring a variable as being thread-local.

      E.g.
      @code
      juce_ThreadLocal int myVariable;
      @endcode
  */
  #define juce_ThreadLocal    __declspec(thread)
#else
  #define juce_ThreadLocal    __thread
#endif


//==============================================================================
/** Clears a block of memory. */
#define zeromem(memory, numBytes)               memset (memory, 0, numBytes)

/** Clears a reference to a local structure. */
#define zerostruct(structure)                   memset (&structure, 0, sizeof (structure))

/** A handy macro that calls delete on a pointer if it's non-zero, and
    then sets the pointer to null.
*/
#define deleteAndZero(pointer)                  { delete (pointer); (pointer) = 0; }



#endif   // __JUCE_MEMORY_JUCEHEADER__
