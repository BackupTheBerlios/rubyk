#ifndef _ERROR_META_METHOD_H_
#define _ERROR_META_METHOD_H_
#include "oscit/root.h"

namespace oscit {

class ErrorMetaMethod : public Object
{
public:
  ErrorMetaMethod(const char * name) : Object(name, ANY_TYPE_TAG_ID) {}

  virtual const Value trigger (const Value &val) {
    std::cerr << val << std::endl;
    return gNilValue;
  }
};

#endif // _ERROR_META_METHOD_H_

} // oscit