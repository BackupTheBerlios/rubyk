#include "mimas.h"
#include "mimas_window_content.h"
#include "m_device_view.h"
#include "m_device_label.h"
#include "m_slider.h"
#include "m_pad.h"
#include "m_theme.h"

#include "m_object_proxy.h"

#define SLIDER_BORDER_WIDTH 2
#define SLIDER_BORDER_RADIUS 6


// =============================================
// ==             Colors                      ==
// =============================================

void MTheme::set_day_theme() {
  colors_[WorkspaceActionBG] = Colour(0xff262626);
  colors_[WorkspaceEditBG]   = Colour(0xff808080);
  colors_[WorkspaceFrozenBG] = Colour(0xff3f0000);
  colors_[WorkspaceBorder]   = Colour(0xff000000);

  colors_[ToolbarBG]         = Colour(0xff808080);
  colors_[DeviceBorder]      = Colour(0xffa0a0a0);
  colors_[DeviceLabel]       = Colour(0xffa0a0a0);
}

void MTheme::set_night_theme() {
  colors_[WorkspaceActionBG] = Colour(0xff262626);
  colors_[WorkspaceEditBG]   = Colour(0xff808080);
  colors_[WorkspaceFrozenBG] = Colour(0xff3f0000);
  colors_[WorkspaceBorder]   = Colour(0xff000000);

  colors_[ToolbarBG]         = Colour(0xff808080);
  colors_[DeviceBorder]      = Colour(0xffa0a0a0);
  colors_[DeviceLabel]       = Colour(0xffa0a0a0);
}


// =============================================
// ==             MimasWindowContent          ==
// =============================================

void MimasWindowContent::paint(Graphics& g) {
  g.fillAll(bg_color());
  g.setColour(color(MTheme::ToolbarBG));
  g.fillRect(0, 0, getWidth(), TOOLBAR_HEIGHT);
  //g.strokePath (internalPath1, PathStrokeType (5.2000f));
}

// =============================================
// ==             MDeviceView                 ==
// =============================================

void MDeviceView::paint(Graphics &g) {
  g.setColour(mimas_->color(MTheme::DeviceBorder).withMultipliedAlpha(hover_ ? 1.0f : 0.5f));

  g.drawRoundedRectangle(
    (DEVICE_BORDER_WIDTH / 2),
    (DEVICE_BORDER_WIDTH / 2) + label_->min_height() / 2,
    getWidth() - DEVICE_BORDER_WIDTH,
    getHeight() - (label_->min_height() / 2) - DEVICE_BORDER_WIDTH,
    DEVICE_ROUNDED_RADIUS,
    DEVICE_BORDER_WIDTH);
}

// =============================================
// ==             MDeviceLabel                ==
// =============================================

void MDeviceLabel::paint(Graphics &g) {
  label_->setColour(Label::textColourId, mimas_->color(MTheme::DeviceLabel));
  label_->setColour(Label::backgroundColourId, mimas_->bg_color());
  Component::paint(g);
}

// =============================================
// ==             MSlider                     ==
// =============================================

/**
 * TODO: use a table lookup for the round_delta ?
 */

void MSlider::paint(Graphics &g) {
  g.fillAll(mimas_->bg_color()); // TODO: do we need this ?

  g.setColour(fill_color_);
  if (slider_type_ == VerticalSliderType) {
    // vertical slider
    int remote_pos = scaled_remote_value(getHeight()-2);
    int h = getHeight() - remote_pos - SLIDER_BORDER_WIDTH / 2;
    // filled slider value
    g.fillRect(
      0,
      h,
      getWidth(),
      remote_pos
    );
    // line on top of value
    g.setColour(border_color_);
    //g.drawLine(
    //  SLIDER_BORDER_WIDTH / 2,
    //  h - SLIDER_BORDER_WIDTH / 2,
    //  getWidth() - SLIDER_BORDER_WIDTH,
    //  h - SLIDER_BORDER_WIDTH / 2,
    //  SLIDER_BORDER_WIDTH
    //);
  } else {
    // horizontal slider
    int remote_pos = scaled_remote_value(getWidth()-2);
    g.fillRect(0, 0, remote_pos, getHeight());
  }
  g.setColour(border_color_);
  g.drawRect(
    0,
    0,
    getWidth(),
    getHeight(),
    SLIDER_BORDER_WIDTH
  );
}

// =============================================
// ==             MPad                        ==
// =============================================

void MPad::paint(Graphics& g) {
  float radius = 8;
  float pos_x;
  float pos_y;

  g.fillAll(Colours::grey);

  if (abs(range_x_.value_ - range_x_.remote_value_) + abs(range_y_.value_ - range_y_.remote_value_) > 4 * radius) {
    // remote_value_
    pos_x = range_x_.scaled_remote_value(getWidth()) - radius;
    pos_y = getHeight() - range_y_.scaled_remote_value(getHeight()) - radius;
    g.setColour(Colours::lightgrey);
    g.fillEllipse(pos_x, pos_y, 2*radius, 2*radius);

    g.setColour(Colours::darkgrey);
    g.drawEllipse(pos_x, pos_y, 2*radius, 2*radius, 2.0f);
  }

  // value_
  pos_x = range_x_.scaled_value(getWidth())  - radius;
  pos_y = getHeight() - range_y_.scaled_value(getHeight()) - radius;
  g.setColour(Colours::white);
  g.fillEllipse(pos_x, pos_y, 2*radius, 2*radius);

  g.setColour(Colours::black);
  g.drawEllipse(pos_x, pos_y, 2*radius, 2*radius, 2.0f);

  g.setColour(Colours::black);
  g.drawRect(
    0,
    0,
    getWidth(),
    getHeight(),
    SLIDER_BORDER_WIDTH
  );
}