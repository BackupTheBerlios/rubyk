#include "class.h"

//////////  Used for testing. Do not modify (Use 'Plus' object). ///////////


class Add : public Node
{
public:
  bool init (const Params& p)
  {
    mValue1 = 0;
    mValue2 = 0;
    return true;
  }
  
  bool set (const Params& p)
  {
    p.get(&mValue1, "value1");
    p.get(&mValue2, "value2");   
    return true;
  }

  virtual void spy() 
  { bprint(mSpy, mSpySize,"%.2f", mValue1 + mValue2 );  }
  
  void bang(const Signal& sig)
  { 
    sig.get(&mValue1);
    send(mValue1 + mValue2);
  }
  
  void value2(const Signal& sig)
  { sig.get(&mValue2); }

  
private:
  double mValue1;
  double mValue2;
};

extern "C" void init()
{
  CLASS (Add)
  INLET (Add, value2)
  OUTLET(Add, sum)
}