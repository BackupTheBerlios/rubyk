#ifndef _BANG_VALUE_H_
#define _BANG_VALUE_H_
#include "value.h"

class Bang;

/* Holds the actual data of the Bang class. This is a wrapper around a real_t. */
class BangData : public Data
{
public:
  DATA_METHODS(BangData, BangValue)

  BangData(const std::string& s) {}
  
  virtual ~BangData() {}
};

/** Value class to hold a single number (real_t). */
class Bang : public Value
{
public:
  VALUE_METHODS(Bang, BangData, BangValue, Value)
  
  // this is the only way to instanciate a bang
  Bang(bool whatever) : Value(new BangData()) {} // TODO: replace by gBangValue.data_pointer() ?
};

#endif // _BANG_VALUE_H_