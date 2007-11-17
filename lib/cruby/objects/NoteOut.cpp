#include "class.h"
#include "midi_message.h"
#include <sstream>

class NoteOut : public Node
{
public:
  bool init(const Params& p)
  {
    mMessage.mType = NoteOn;
    mMessage.set_note(     p.val("note",     MIDI_NOTE_C0   ));
    mMessage.set_velocity( p.val("velocity", 70             ));
    mMessage.mLength    =  p.val("length",   500             ); // 0.5 sec.
    mMessage.set_channel(  p.val("channel",  1              ));
    
    return true;
  }
  
  // inlet 1 and 5 (silent set note)
  void bang(const Signal& sig)
  {
    if (sig.type == MidiSignal) {
      mMessage = *(sig.midi_ptr.value);
    } else {
      set_note(sig);
    }
    send(mMessage);
  }

  // inlet 2
  void set_velocity(const Signal& sig)
  {
    int v = 0;
    sig.get(&v);
    if (v) mMessage.set_velocity(v); 
  }
  
  // inlet 3
  void set_length(const Signal& sig)
  { sig.get(&(mMessage.mLength)); }
  
  // inlet 4
  void set_channel(const Signal& sig)
  {
    int i;
    if (sig.get(&i)) mMessage.set_channel(i);
  }
  
  // inlet 5 (set note but do not send)
  void set_note(const Signal& sig)
  {
    int n;
    if (sig.get(&n)) mMessage.set_note(n);
  }
  
  // internal callback
  void noteOff(void * data)
  {
    MidiMessage * msg = (MidiMessage*)data;
    Outlet * out;
    Signal sig;
    
    sig.set(msg);
    if (out = outlet(1)) out->send(sig);
    delete msg;
  }
  
  void clear()
  { remove_my_events(); }
  
  virtual void spy()
  { 
    std::ostringstream oss(std::ostringstream::out);
    oss << mMessage;
    bprint(mSpy, mSpySize,"%s", oss.str().c_str());
  }
  
private:
  /* data */
  MidiMessage  mMessage;
  time_t mLength;
};

extern "C" void init()
{
  CLASS (NoteOut)
  INLET (NoteOut, set_velocity)
  INLET (NoteOut, set_length)
  INLET (NoteOut, set_channel)
  INLET (NoteOut, set_note)
  OUTLET(NoteOut, send)
  METHOD(NoteOut, clear)
}