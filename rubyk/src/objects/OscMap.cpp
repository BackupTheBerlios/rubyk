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

#include <list>
#include <stdexcept> // runtime_error

#include "oscit.h"
#include "rubyk.h"
#include "oscit/osc_map_command.h"

#include "ip/UdpSocket.h"

class OscMap : public Node, public OscMapCommand {
public:
  OscMap() {}
  
  virtual ~OscMap() {
    kill();
  }
  
  virtual const Value start() {
    // needed so that it's not Thread::start that is called...
    return gNilValue;
  }
};

extern "C" void init(Planet &planet) {
  CLASS (OscMap, "Open udp ports and map calls from these ports.", "script: [mapping definitions] or file: [path to mapping file]")
  // {1}
  c->add_method<Script, &Script::file>("file", StringIO("path", "Set path to mappings definitions."));
  // {2}
  c->add_method<Script, &Script::script>("script", StringIO("mappings", "Mappings definitions."));
  // {3}
  c->add_method<Script, &Script::reload>("reload", RealIO("seconds", "How often shoudl we check file for relaod."));
  
  c->add_method<OscMapCommand, &OscMapCommand::port>("number", RealIO("number", "Set incomming port number."));
  c->add_inlet<OscMapCommand,  &OscMapCommand::port>("number", RealIO("number", "Set incomming port number."));
  c->add_method<OscMapCommand, &OscMapCommand::reply_port>("number", RealIO("number", "Set outgoing reply port number."));
  c->add_inlet<OscMapCommand,  &OscMapCommand::reply_port>("number", RealIO("number", "Set outgoing reply port number."));
}
