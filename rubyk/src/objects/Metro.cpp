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

#include "rubyk.h"

class Metro : public Node
{
public:
  Metro() : tempo_(120), run_(false) {}
  
  const Value start() {
    run_ = true;
    bang(gNilValue); // start loop
    return gNilValue;
  }
  
  // [1] restart metronome / set tempo
  const Value tempo(const Value &val) {
    if (val.is_real()) change_tempo(val.r);
    
    return Value(tempo_);
  }
  
  // [2] stop metronome
  const Value start_stop(const Value &val) {
    if (val.is_real()) {
      if (val.r == 0.0) {
        // stop
        remove_my_events();
        run_ = false;
      } else {
        // start
        if (!run_) bang_me_in(ONE_MINUTE / tempo_);
        run_ = true;
      }
    }
    
    return Value(run_);
  }
  
  virtual void inspect(Value *hash) const {
    hash->set("tempo", tempo_);
    hash->set("run", run_);
  }
  
  // internal use only (looped call)
  void bang(const Value &val) {
    if (run_) {
      // TODO: optimize: reuse previous event (send it in Value& val as EventValue ?)
      bang_me_in(ONE_MINUTE / tempo_);
      send(gNilValue);
    }
  }
  
private:
  void change_tempo(Real tempo) {
    if (tempo <= 0) {
      run_ = false;
    } else if (tempo != tempo_) {
      // tempo changed
      tempo_ = tempo;
      if ((ONE_MINUTE / tempo_) < (WORKER_SLEEP_MS + 1)) {
        tempo_ = ONE_MINUTE / (WORKER_SLEEP_MS * 1.5);
      }
      remove_my_events();
      if (run_) bang_me_in(ONE_MINUTE / tempo_);
    }
  }
  
  Real tempo_;
  bool run_;
};

extern "C" void init(Planet &planet) {
  CLASS( Metro, "Metronome that sends bangs at regular intervals.", "tempo: [initial tempo]")
  METHOD(Metro, tempo,RealIO("bpm", "Restart metronome | set tempo value."))
  METHOD(Metro, start_stop, RealIO("1,0", "Start/stop metronome."))
  OUTLET(Metro, bang, BangIO("Regular bangs."))
}