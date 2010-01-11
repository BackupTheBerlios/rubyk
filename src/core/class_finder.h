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

#ifndef _CLASS_FINDER_H_
#define _CLASS_FINDER_H_
#include "oscit.h"
#include "new_method.h"
#include "class.h"

/** Special class to handle class listing from a directory. This usually responds at to the '/class' url. */
class ClassFinder : public Object
{
public:
  TYPED("Object.ClassFinder")

  ClassFinder(const std::string &name, const char *objects_path) : Object(name), objects_path_(objects_path) {
    init();
  }

  ClassFinder(const std::string &name, std::string &objects_path) : Object(name), objects_path_(objects_path) {
    init();
  }

  ClassFinder(const char *name, const char *objects_path) : Object(name), objects_path_(objects_path) {
    init();
  }

  ClassFinder(const char *name, std::string &objects_path) : Object(name), objects_path_(objects_path) {
    init();
  }

  void init() {
    //          /class/lib
    adopt(new TMethod<ClassFinder, &ClassFinder::lib_path>(this, Url(LIB_URL).name(), StringIO("file path", "Get/set path to load objects files (*.rko).")));
  }

  virtual ~ClassFinder() {}

  /** This trigger implements "/class". It returns the list of objects in objects_path_. */
  virtual const Value trigger (const Value &val);

  virtual Object *build_child(const std::string &class_name, const Value &type, Value *error);

  const Value lib_path(const Value &val) {
    if (val.is_string()) objects_path_ = val.str();
    return Value(objects_path_);
  }

  /** Declare a new class. This template is responsible for generating the "new" method. */
  template<class T>
  Class * declare(const char *name, const char *info, const char *options)
  {
    Class * klass;

    if (find_class(name))
      delete klass; // remove existing class with same name.

    klass = adopt(new Class(name, NoIO(info)));

    if (!klass) return NULL; // FIXME: this will crash !!!

    // build "new" method for this class
    klass->adopt(new NewMethod( "new", &NewMethod::cast_create<T>,
                                AnyIO(std::string("Create a new ").append(name).append(" from a given url and optional Hash of parameters (").append(options).append(")."))));

    return klass;
  }

  /** Get a Class object from it's name ("Metro"). */
  Class * find_class (const char *name) {
    return TYPE_CAST(Class, child(name));
  }

  /** Get a Class object from it's std::string name ("Metro"). */
  Class * find_class (const std::string &name) {
    return TYPE_CAST(Class, child(name));
  }

private:
  /** Load an object stored in a dynamic library. */
  bool load(const char * file, const char * init_name);

  std::string objects_path_; /**< Where to find objects in the filesystem. */
};

#endif // _CLASS_FINDER_H_