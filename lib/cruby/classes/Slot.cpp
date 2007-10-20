#include "slot.h"
#include "node.h"

void Slot::setId(int pId)
{
  mId = pId;
}

void Slot::connect(Slot * pOther)
{
  add_connection(pOther);
  pOther->add_connection(this);
}

void Slot::disconnect(Slot * pOther)
{
  remove_connection(pOther);
  pOther->remove_connection(this);
}
    

/** Sort slots by rightmost node and rightmost position in the same node. */
inline bool Slot::operator>= (const Slot& pOther) const
{
  if (mNode == pOther.mNode) {
    // same node, sort by position in container, largest first
    return mId < pOther.mId;
  } else {
    // different node, sort by node position, greatest first
    return ((Node*)(mNode))->trigger_position() < ((Node*)(pOther.mNode))->trigger_position();
  }
}


void Slot::add_connection(Slot * pOther)
{
  mConnections.push(pOther); /** OrderedList makes sure the link is not created again if it already exists. */
}

void Slot::remove_connection(Slot * pOther)
{
  mConnections.remove(pOther);
}