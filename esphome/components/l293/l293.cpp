#include "l293.h"
#include "esphome/core/log.h"

namespace esphome {
namespace l293 {

static const char *TAG = "l293.stepper";

void L293::setup() {
  this->pin_a_->setup();
  this->pin_b_->setup();
  this->pin_en1_->setup();
  this->pin_en2_->setup();
  this->loop();
}

void L293::loop() {
  bool at_target = this->has_reached_target();
  if (at_target) {
    this->high_freq_.stop();

    if (this->sleep_when_done_) {
      this->pin_en1_->digital_write(false);
      this->pin_en2_->digital_write(false);
      // do not write pos
      return;
    }
  } else {
    this->high_freq_.start();

    int dir = this->should_step_();
    this->current_l_pos_ += dir;
  }

  this->write_step_(this->current_l_pos_);
}

void L293::dump_config() {
  ESP_LOGCONFIG(TAG, "L293:");
  LOG_PIN("  Pin A: ", this->pin_a_);
  LOG_PIN("  Pin B: ", this->pin_b_);
  LOG_PIN("  Pin EN1: ", this->pin_en1_);
  LOG_PIN("  Pin EN2: ", this->pin_en2_);
  ESP_LOGCONFIG(TAG, "  Sleep when done: %s", YESNO(this->sleep_when_done_));
  const char *step_mode_s = "";
  switch (this->step_mode_) {
    case L293_STEP_MODE_FULL_STEP:
      step_mode_s = "FULL STEP";
      break;
    case L293_STEP_MODE_HALF_STEP:
      step_mode_s = "HALF STEP";
      break;
    case L293_STEP_MODE_WAVE_DRIVE:
      step_mode_s = "WAVE DRIVE";
      break;
  }
  ESP_LOGCONFIG(TAG, "  Step Mode: %s", step_mode_s);
}

void L293::write_step_(int32_t step) {
  constexpr int o = 0; // value that does not matter
  static const std::vector<std::vector<int>> full_step = {
      { 0, 0, 1, 1 },
      { 0, 1, 1, 1 },
      { 1, 1, 1, 1 },
      { 1, 0, 1, 1 },
  };
  static const std::vector<std::vector<int>> half_step = {
      { 0, 0, 1, 1 },
      { o, 0, 0, 1 },
      { 1, 0, 1, 1 },
      { 1, o, 1, 0 },
      { 1, 1, 1, 1 },
      { o, 1, 0, 1 },
      { 0, 1, 1, 1 },
      { 0, o, 1, 0 },
  };
  static const std::vector<std::vector<int>> wave_drive = {
      { 0, o, 1, 0 },
      { o, 1, 0, 1 },
      { 1, o, 1, 0 },
      { o, 0, 0, 1 },
  };

  const std::vector<std::vector<int>> *step_table = 0;
  int32_t n = this->step_mode_ == L293_STEP_MODE_HALF_STEP ? 8 : 4;
  const auto i = step % n;
  uint8_t res = 0;

  switch (this->step_mode_) {
    case L293_STEP_MODE_FULL_STEP:
      step_table = &full_step;
      break;
    case L293_STEP_MODE_HALF_STEP:
      step_table = &half_step;
      break;
    case L293_STEP_MODE_WAVE_DRIVE:
      step_table = &wave_drive;
      break;
  }

  //ESP_LOGCONFIG(TAG, "Step %d%%%d=%d: %d %d %d %d", step, n, i, (*step_table)[i][0], (*step_table)[i][1], (*step_table)[i][2], (*step_table)[i][3]);
  this->pin_a_->digital_write(  (*step_table)[i][0]);
  this->pin_b_->digital_write(  (*step_table)[i][1]);
  this->pin_en1_->digital_write((*step_table)[i][2]);
  this->pin_en2_->digital_write((*step_table)[i][3]);
}

}  // namespace l293
}  // namespace esphome
