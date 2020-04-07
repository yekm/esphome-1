#pragma once

#include "esphome/core/component.h"
#include "esphome/core/esphal.h"
#include "esphome/components/stepper/stepper.h"

namespace esphome {
namespace l293 {

enum L293StepMode {
  L293_STEP_MODE_FULL_STEP,
  L293_STEP_MODE_HALF_STEP,
  L293_STEP_MODE_WAVE_DRIVE,
};

class L293 : public stepper::Stepper, public Component {
 public:
  void set_pin_a(GPIOPin *pin_a) { pin_a_ = pin_a; }
  void set_pin_b(GPIOPin *pin_b) { pin_b_ = pin_b; }
  void set_pin_en1(GPIOPin *pin_en1) { pin_en1_ = pin_en1; }
  void set_pin_en2(GPIOPin *pin_en2) { pin_en2_ = pin_en2; }

  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  void set_sleep_when_done(bool sleep_when_done) { this->sleep_when_done_ = sleep_when_done; }
  void set_step_mode(L293StepMode step_mode) { this->step_mode_ = step_mode; }

 protected:
  void write_step_(int32_t step);

  bool sleep_when_done_{false};
  GPIOPin *pin_a_;
  GPIOPin *pin_b_;
  GPIOPin *pin_en1_;
  GPIOPin *pin_en2_;
  L293StepMode step_mode_{L293_STEP_MODE_FULL_STEP};
  HighFrequencyLoopRequester high_freq_;
  int32_t current_l_pos_{0};
};

}  // namespace l293
}  // namespace esphome
