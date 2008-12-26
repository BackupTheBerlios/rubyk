#ifndef _OSC_OBJECT_H_
#define _OSC_OBJECT_H_
#include <list>
#include "values.h"
#include "thash.h"


namespace oscit {
#define OSC_NEXT_NAME_BUFFER_SIZE 20

class Root;
class Alias;

class Object
{
public:
  Object() : mRoot(NULL), mParent(NULL), mChildren(20)
  {
    mName = default_name();
    mUrl  = mName;
  }

  Object(const char * pName) : mRoot(NULL), mParent(NULL), mChildren(20)
  {
    mName = pName;
    mUrl  = mName;
  }

  Object(const std::string& pName) : mRoot(NULL), mParent(NULL), mChildren(20)
  {
    mName = pName;
    mUrl  = mName;
  }

  Object(Object * pParent) : mRoot(NULL), mParent(NULL), mChildren(20)
  {
    mName = default_name();
    pParent->adopt(this);
  }

  Object(Object * pParent, const char * pName) : mRoot(NULL), mParent(NULL), mChildren(20)
  {
    mName = pName;
    pParent->adopt(this);
  }

  Object(Object * pParent, const std::string& pName) : mRoot(NULL), mParent(NULL), mChildren(20)
  {
    mName = pName;
    pParent->adopt(this);
  }

  Object(Object& pParent) : mRoot(NULL), mParent(NULL), mChildren(20)
  {
    mName = default_name();
    pParent.adopt(this);
  }

  Object(Object& pParent, const char * pName) : mRoot(NULL), mParent(NULL), mChildren(20)
  {
    mName = pName;
    pParent.adopt(this);
  }

  Object(Object& pParent, const std::string& pName) : mRoot(NULL), mParent(NULL), mChildren(20)
  {
    mName = pName;
    pParent.adopt(this);
  }

  virtual ~Object();

  /** Shortcut to call multiple methods on an object.
    * Using "obj.set(foo:4 bar:5)" is equivalent to calling "obj.foo(4)" and "obj.bar(5)". */
  bool set(const Value& pParams)
  {
    Object * obj;
    if (pParams.is_hash()) {
      Hash p(pParams);
      Hash_iterator it;
      Hash_iterator end = p.end();
  
      for(it = p.begin(); it != end; it++) {
        if ( (obj = child(*it)) ) {
          obj->trigger(p[*it]);
        }
      }
    } else {
      // use first method as default
      string_iterator it  = mChildren.begin();
      string_iterator end = mChildren.end();
      if (it != end && (obj = child(*it))) {
        obj->trigger(pParams);
      }
    }
    return true;
  }

  /** This is the prefered way to insert new objects in the tree since it clearly highlights ownership in the parent. 
    * TODO: make sure a parent is not adopted by it's child. */
  template<class T>
  T * adopt(T * pObject)
  {
    do_adopt(pObject);
    return pObject;
  }
  
  /** Class signature. */
  virtual uint type()
  {
    return H("oscit::Object");
  }

  template<class T>
  static inline T * type_cast(uint pType, Object * obj)
  {
    return (obj && obj->type() == pType) ? (T*)obj : NULL;
  }

  /** Clear all children (delete). */
  virtual void clear();

  /** The operation to be executed on call (method for Method class / object listing for Nodes).
    * TODO: default is "return gNilValue" in new oscit definition. */
  virtual const Value trigger (const Value& val)
  {
    std::string res;
    string_iterator it  = mChildren.begin();
    string_iterator end = mChildren.end();
    bool start = true;
    if (it == end) return gNilValue;

    while(it != end) {
      Object * obj;
      if (mChildren.get(&obj, *it)) {
        if (obj->type() != H("Alias")) {
            // do not list alias (Alias are used as internal helpers and do not need to be advertised)
          if (!start) res.append(",");
          res.append(obj->mName);
          if (!(obj->mChildren.empty())) res.append("/");
          start = false;
        }
      }
      it++;
    }
  
    return String(res);
  }

  /** This method is called whenever a sub-node or branch is not found and this is the last found object along the path. */
  virtual const Value not_found (const std::string& pUrl, const Value& val)
  {
    return Error(std::string("Object '").append(pUrl).append("' not found."));
  }
  
  /** Define the object's container (parent). */
  void set_parent(Object& parent)
  { set_parent(&parent); }

  /** Define the object's container (parent). */
  void set_parent(Object * parent)
  {
    if (!parent) {
      if (mParent) mParent->release(this);
      mParent = NULL;
      moved();
    } else {
      parent->adopt(this);
    }
  }

  /** Inform the object of an alias linked to this object (has to be deleted on destruction). */
  void register_alias (Alias * pAlias);

  /** Inform the object that an alias no longer exists. */
  void unregister_alias (Alias * pAlias);

  /** Return the object's unique url. */
  inline const std::string& url ()
  {
    return mUrl;
  }
  
  /** Register unique url in the Root object so that methods addressed to this url actually end in this object. */
  void register_url ();
  
  /** Set name from string. If the name is not unique in the parent's scope, the name is changed as "name-1", "name-2", etc. */
  void set_name (const char* pName) 
  { set_name(std::string(pName)); }


  /** Set name from string. If the name is not unique in the parent's scope, the name is changed as "name-1", "name-2", etc. */
  void set_name (const std::string& pName) 
  { 
    if (pName == "") return;
    mName = pName; // FIXME: gsub(/[^a-zA-Z\-0-9_],"")
  
    moved();
  }
  
  
  /* ========================== REPLIES TO META METHODS =========================== */
  /* The replies to meta methods are implemented as virtuals so that objects that   */
  /* inherit from osc::Object just need to overwrite these in order to return more  */
  /* meaningful information / content.                                              */
  
  /** Inspection method. Called as a response to "/.inspect '/this/url'". */
  virtual const Value inspect(const Value& val)
  {
    return String(mInfo);
  }
  
  /** Human readable information method. Called as a response to "/.info '/this/url'". */
  const std::string& info() const
  {
    return mInfo;
  }

  /** Set info string. */
  void set_info (const char* pInfo)
  {
    mInfo = pInfo;
  }

  /** Set info string. */
  void set_info (const std::string& pInfo)
  {
    mInfo = pInfo;
  }
  
  /** Return the list of children as a hash. */
  const THash<std::string,Object *> children() const
  {
    return mChildren;
  }

  /** Return first child. */
  Object * first_child()
  {
    return mChildren.size() > 0 ? child(mChildren.keys().front()) : NULL;
  }

  /** Return the direct child named 'pName'. */
  Object * child(const std::string& pName)
  {
    Object * child;
    if (mChildren.get(&child, pName)) {
      return child;
    } else {
      return NULL;
    }
  }

  /** Actual adoption. Adopt objects in the new namespace (branch). */
  virtual void do_adopt(Object * pObject)
  {
    Object * oldParent = pObject->mParent;

    if (oldParent) oldParent->release(pObject);

    pObject->mParent = this;
    pObject->mRoot   = mRoot;

    pObject->moved();
  }

protected:

  /** Child sends a notification to the parent when it's name changes so that the parent/root keep their url hash in sync. */
  void register_child(Object * pChild);

  /** Update cached url, notify mRoot of the position change. */
  void moved();

  /** Add '-1', '-2', ... at the end of the current name. bob --> bob-1 */
  void next_name()
  {
    size_t pos = mName.find('-');
    char buffer[OSC_NEXT_NAME_BUFFER_SIZE];
    if (pos == std::string::npos) {
      mName.append("-1");
    } else {
      std::string baseName = mName.substr(0,pos - 1);
      int i = atoi(mName.substr(pos + 1, std::string::npos).c_str());
      snprintf(buffer, OSC_NEXT_NAME_BUFFER_SIZE, "%i", i+1);
      mName = baseName.append(buffer);
    }
  }

private:
  
  // TODO: this could be replaced by "o" ("o-1", "o-2", "o-3") ? 
  std::string default_name()
  {
    char buf[50];
    sIdCounter++;
    sprintf(buf,"_%i",sIdCounter);
    return std::string(buf);  // default variable name is 'id'
  }
  
  /** Free the child from the list of children. */
  void release(Object * pChild);

  std::list<Alias *>          mAliases;          /**< List of aliases to destroy when this node disappears. */

protected:
  friend class Root;

  Root   *                    mRoot;             /**< Root object. */
  Object *                    mParent;           /**< Pointer to parent object. */
  THash<std::string,Object *> mChildren;         /**< Hash with pointers to sub-objects / methods */
  std::string                 mUrl;              /**< Absolute path to object (cached). TODO: this cache is not really needed. */
  std::string                 mName;             /**< Unique name in parent's context. */
  std::string                 mInfo;             /**< Help/information string. */
  static unsigned int         sIdCounter;        /**< Use to set a default id and position. */

};
} // namespace osc

#define TYPE_CAST(klass, op) oscit::Object::type_cast<klass>(H(#klass), op);

#endif // _OSC_OBJECT_H_