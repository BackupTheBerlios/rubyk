#include "mimas.h"
#include "m_device_view.h"

#include "m_device_label.h"
#include "m_slider.h"
#include "m_pad.h"

#define PARTS_HASH_SIZE 100

MDeviceView::MDeviceView(const std::string &part_id, MViewProxy *view_proxy, const std::string &name)
    : MComponent(part_id, view_proxy),
      parts_(PARTS_HASH_SIZE) {
  label_ = new MDeviceLabel(mimas_, this, T("device name"), String(name.c_str()));

  border_ = new ResizableBorderComponent(this, 0);
  addAndMakeVisible(border_);
  addAndMakeVisible(label_);
  resized();
}

//MDeviceView::~MDeviceView() {
//
//}

void MDeviceView::update(const Value &def) {
  MComponent::update(def);

  Value parts_value = def["parts"];
  if (!parts_value.is_hash()) {
    error("'parts' attribute is not a hash. Found", parts_value);
    return;
  }

  Hash *parts = parts_value.hash_;
  Hash::const_iterator it, end = parts->end();
  Value part_def;
  MComponent *part;

  for (it = parts->begin(); it != end; ++it) {
    if (parts->get(*it, &part_def) && part_def.is_hash()) {
      if (parts_.get(*it, &part)) {
        // part already exists, update
      } else {
        // create new part from definition
        Value klass = part_def["class"];
        if (!klass.is_string()) {
          error("'class' attribute missing in", part_def);
          continue;
        }

        if (klass.str() == "Slider") {
          // ================================================== Slider
          part = new MSlider(*it, view_proxy_);

        } else if (klass.str() == "Pad") {
          // ================================================== Pad
          part = new MPad(*it, view_proxy_);

        } else {
          part = NULL;
          error("Unknown class", klass);
        }
      }

      if (part) {
        part->update(part_def);
        if (!part->isVisible()) addAndMakeVisible(part);
        parts_.set(*it, part);
      }
    }
  }

}

void MDeviceView::resized() {
  // FIXME: get label width...
  border_->setBounds(
    (DEVICE_BORDER_WIDTH / 2),
    (DEVICE_BORDER_WIDTH / 2) + label_->min_height() / 2,
    getWidth() - DEVICE_BORDER_WIDTH,
    getHeight() - (label_->min_height() / 2) - DEVICE_BORDER_WIDTH
  );
  //  device_browser_->setBounds (10, 10, getWidth() - 10, getHeight() - 250);
  //  quitButton->setBounds (getWidth() - 176, getHeight() - 60, 120, 32);
  //  workspace_->setBounds (10, getHeight() - 250, getWidth() - 20, 200);
}