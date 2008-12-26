#include "oscit/root.h"

namespace oscit {
Root::~Root()
{
  std::list<OscSend*>::iterator it  = mControllers.begin();
  std::list<OscSend*>::iterator end = mControllers.end();

  delete mOscIn;

  while (it != end) delete *it++;
}
} // namespace oscit