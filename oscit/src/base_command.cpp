#include "oscit/base_command.h"
#include "oscit/root.h"

namespace oscit {

BaseCommand::~BaseCommand() {
  kill();
  if (root_) {
    root_->unregister_command(this);
    root_ = NULL;
  }
}

} // oscit