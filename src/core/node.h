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

#ifndef _NODE_H_
#define _NODE_H_
#include "oscit.h"
#include "event.h"
#include "inlet.h"
#include "outlet.h"
#include "worker.h"

#include <sstream>
#include <cstdio>
#include <vector>
#include <string>
#include <list>
#include <pthread.h>

class Observer;

/** Base class for all nodes in rubyk.
 *
 *  Initialization is done in the following order:
 *  1. create : you should not write constructors.
 *  2. init   : return error messages if this step fails.
 *  2. set    : call methods with parameters, return error messages if this fails.
 *  3. start  : this is called only if init and set did not fail.
 */
class Node : public Object {
 public:
  TYPED("Object.Node")

  Node() : Object("n", AnyIO("Node.")), worker_(NULL), looped_(false) {
    trigger_position_ = ++sIdCounter; // FIXME: atomic operation
  }

  virtual ~Node();

  /** Trigger for nodes calls methods if hash given or tries to call first inlet. */
  virtual const Value trigger(const Value &val) {
    if (val.is_hash()) {
      return set(val);
    } else if (!val.is_nil()) {
      // call first method
      Object *child = first_child();
      if (child && child->can_receive(val)) {
        return child->trigger(val);
      } else {
        return Value(BAD_REQUEST_ERROR, std::string("Invalid arguments for first method '").append(child->name()).append("'."));
      }
    }
    return do_inspect();
  }

  const Value do_inspect() const {
    HashValue hash;
    inspect(&hash);
    return Value(std::string("<").append(class_url().substr(strlen(CLASS_URL)+1)).append(":").append(url()).append(" ").append(hash.lazy_json()).append(">"));
  }

  virtual void inspect(Value *hash) const {}

  /** Add an inlet with the given callback (used by Class during instantiation). */
  void register_inlet(Inlet *inlet) {
    inlet->set_id(inlets_.size()); /* first inlet has id 0 */
    inlets_.push_back(inlet);
  }

  /** Add an outlet. (used by Class during instantiation). */
  void register_outlet(Outlet *outlet) {
    outlet->set_id(outlets_.size()); /* first outlet has id 0 */
    outlets_.push_back(outlet);
  }

  /** Remove inlet from inlets list of callbacks. */
  void unregister_inlet(Inlet * inlet) {
    std::vector<Inlet*>::iterator it;
    std::vector<Inlet*>::iterator end = inlets_.end();

    for(it = inlets_.begin(); it != end; it++) {
      if (*it == inlet) {
        inlets_.erase(it);
        break;
      }
    }
  }

  /** Remove outlet from outlets list of callbacks. */
  void unregister_outlet(Outlet * outlet) {
    std::vector<Outlet*>::iterator it;
    std::vector<Outlet*>::iterator end = outlets_.end();

    for(it = outlets_.begin(); it != end; it++) {
      if (*it == outlet) {
        outlets_.erase(it);
        break;
      }
    }
  }

  /**< Sending order from incoming connections should be updated. */
  void sort_connections();

  /** When this method is implemented in subclasses, it is used to
   *  do a basic setup with default parameters before these
   *  are changed by calling methods during runtime.
   */
  virtual const Value init() { return gNilValue; }

  /** When this method is implemented in subclasses, it is used as
   *  the last call in the initialization chain.
   */
  virtual const Value start() { return gNilValue; }

  /** Set is_ok_ flag. If this flag is not true, the node is considered "broken" and usually does not do any processing. */
  void set_is_ok(bool status) { is_ok_ = status; }

  /** Return true if the node is not broken.
   */
  bool is_ok() const { return is_ok_; }

  /** This method must be implemented in subclasses that want to be looped.
   */
  virtual void bang(const Value &val) {
    fprintf(stderr, "Default Node::bang method called !\n");
  }

  /** Set url for class. TODO: Maybe we should pass a pointer to the class in case it moves ?
   * But then if it is removed ?
   */
  void set_class_url(const std::string &class_url) { class_url_ = class_url; }

  const std::string &class_url() const { return class_url_; }

  /** Used to sort outlet connections. A node with a high trigger position receives the value before
   * another node with a small trigger position, if they are both connected to the same outlet.
   */
  inline Real trigger_position() { return trigger_position_; }

  void set_trigger_position(Real value) {
    trigger_position_ = value;
    sort_connections();
  }

  /** Send a Value out of the first outlet. */
  inline void send(const Value &val) { send(1, val); }

  /** Send a Value out of an outlet. */
  inline void send (size_t port, const Value &val) {
    if (port > outlets_.size() || port < 1) return;
    outlets_[port - 1]->send(val);
  }

  /** Return the outlet at the given port if there is one and
   *  it can receive the provided value.
   */
  Outlet *outlet_for_value(size_t port, const Value &val) {
    if (port > outlets_.size() || port < 1) return NULL;
    Outlet *outlet = outlets_[port - 1];
    return outlet->can_receive(val) ? outlet : NULL;
  }

  /** Remove all events concerning this node for the events queue. */
  inline void remove_my_events() {
    if (worker_) worker_->free_events_for(this);
  }

  /** Ask to receive a bang in the given interval in [ms]. */
  inline void bang_me_in(time_t interval) {
    worker_->register_event(new BangEvent(this, worker_->current_time_ + interval));
  }

  /** Ask to receive a bang with the given parameter in the given interval in [ms]. */
  inline void bang_me_in(time_t interval, const Value &parameter, bool forced = false) {
    worker_->register_event(new BangEvent(this, worker_->current_time_ + interval, parameter, forced));
  }

  /** Bang me on every loop. */
  inline void loop_me() {
    if (!looped_) {
      worker_->register_looped_node(this);
      looped_ = true;
    }
  }

  /** Stop banging me on every loop. */
  inline void unloop_me() {
    if (looped_) {
      worker_->free_looped_node(this);
      looped_ = false;
    }
  }

  /** Cast general Mutex context to Worker. */
  virtual void set_context(Mutex *context) {
    // FIXME: what do we do if context is NULL ?? (no more parent)
     // TODO: can we use a static_cast here ?
    context_ = context;
    if (context_ != NULL) {
      worker_  = TYPE_CAST(Worker, context_);
      if (context_ == NULL) {
        fprintf(stderr, "Could not cast '%s' to Worker in %s ! Program might crash.\n", context_->class_path(), class_path());
      }
    }
  }

 protected:
  Worker * worker_;  /**< Worker that will give life to object. */

 private:
  static size_t    sIdCounter;   ///< Used to set a default trigger position.

  bool is_ok_;                   /**< If something bad arrived to the node during initialization or edit, the node goes into
                                  *   broken state and is_ok_ becomes false. In 'broken' mode, the node does nothing. */
  bool looped_;                  /**< Set to true if the node is currently called on every worker loop. */

  Real trigger_position_;        /**< When sending signals from a particular slot, a node with a small trigger_position_
                                  *   will receive the signal after a node that has a greater trigger_position_. */
  std::string class_url_;        /**< Url for the node's class. */

  std::vector<Inlet*>  inlets_;  /**< List of inlets. FIXME: is this used ? */
  std::vector<Outlet*> outlets_; /**< List of outlets. */
};

#endif // _NODE_H_