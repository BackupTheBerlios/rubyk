#ifndef OSCIT_TEST_MOCK_DUMMY_COMMAND_H_
#define OSCIT_TEST_MOCK_DUMMY_COMMAND_H_
#include "oscit/command.h"
#include "mock/dummy_object.h"

// FIXME: can we remove this and replace it by CommandLogger ?

struct DummyCommand : public Command
{
 public:
  DummyCommand(std::string *string) : Command("dummy"), string_(string), dummy_host_("dummy", "dummy.host", 2009) {}
  DummyCommand(std::string *string, const char *protocol) : Command(protocol), string_(string), dummy_host_(protocol, "dummy.host", 2009) {}

  ~DummyCommand() {
    // make sure command is halted before being destroyed
    kill();
  }

  virtual void listen() {
    thread_ready();
    while (should_run()) {
      lock();
        string_->append(".");
      unlock();
      millisleep(20);
    }
  }

  virtual void notify_observers(const char *path, const oscit::Value &val) {
    //log("notify", path, val);
  }

  virtual void send(const Location &remote_endpoint, const char *path, const Value &val) {
    //log("send", remote_endpoint, path, val);
  }

  virtual Object *build_remote_object(const Url &url, Value *error) {
    if (url.location() == dummy_host_) {
      return adopt_remote_object(url.str(), new DummyObject(url.str().c_str(), url.port()));
    } else {
      error->set(BAD_REQUEST_ERROR, std::string("Unknown location '").append(url.location().name()).append("'."));
      return NULL;
    }
  }

  Object *remote_object_no_build(const std::string &url) {
    Object *res = NULL;
    remote_objects_.get(url, &res);
    return res;
  }

  const std::list<Location> &observers() const {
    return Command::observers();
  }

  std::string *string_;
  Location dummy_host_;
};

#endif // OSCIT_TEST_MOCK_DUMMY_COMMAND_H_
