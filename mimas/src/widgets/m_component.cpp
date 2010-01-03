#include "mimas.h"
#include "m_component.h"

#include "m_view_proxy.h"

MComponent::MComponent(const std::string &part_id, MViewProxy *view_proxy)
    : part_id_(part_id), // FIXME: replace part_id by def_path...
      mimas_(view_proxy->mimas()),
      view_proxy_(view_proxy),
      root_proxy_(view_proxy->root_proxy()),
      ghost_component_(this),
      hue_(80) {}

void MComponent::update(const Value &def) {

  // ========================================== x, y, width, height

  if (def.has_key("x") || def.has_key("y") || def.has_key("width") || def.has_key("height")) {
    setBounds(
      def["x"].get_real(getX()),
      def["y"].get_real(getY()),
      def["width"].get_real(getWidth()),
      def["height"].get_real(getHeight())
    );
    if (ghost_component_.isVisible()) { // TODO:  && !is_dragged_
      ghost_component_.setVisible(false);
    }
  }

  // ========================================== hue
  if (def.has_key("hue")) {
    set_hue(def["hue"].get_real());
  }
}

void MComponent::set_hue(float hue) {
  hue_ = (hue < 0 || hue >= 360) ? 0 : hue;
  //                     hue            sat   bri   alpha
  border_color_ = Colour(hue_ / 360.0f, 1.0f, 1.0f, isEnabled() ? 1.0f : 0.3f);
  fill_color_   = Colour(hue_ / 360.0f, 0.5f, 0.5f, isEnabled() ? 1.0f : 0.3f);
}

class MComponent::OnRegistrationCallback : public TCallback<MComponent, &MComponent::on_registration_callback> {
public:
  OnRegistrationCallback(MComponent *observer, const Value &def)
      : TCallback<MComponent, &MComponent::on_registration_callback>(observer, new Value(def.to_json())) {}

  virtual ~OnRegistrationCallback() {
    Value *def = (Value*)data_;
    delete def;
  }
};

void MComponent::add_callback(const std::string &path, const Value &def) {
  std::cout << "Add callback '" << path << "' with " << def << "\n";
  root_proxy_->adopt_callback_on_register(path,
    new MComponent::OnRegistrationCallback(this, def)
  );
}

void MComponent::on_registration_callback(void *data) {
  Value *def = (Value*)data;
  MessageManagerLock mml;
  std::cout << "Callback triggered with " << *def << "\n";
  update(*def);
}

