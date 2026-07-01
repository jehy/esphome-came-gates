#include "came_gates_controller.h"

#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace came_gates_controller {

static const char *const TAG = "came_gates_controller.switch";

void CameGatesSwitch::setup() {
  if (pin_ == nullptr) {
    this->mark_failed();
    return;
  }

  pin_->setup();
  pin_->digital_write(false);
}

void CameGatesSwitch::dump_config() {
  LOG_SWITCH("", "CAME Gates Switch", this);
  LOG_PIN("  Pin: ", pin_);
  ESP_LOGCONFIG(TAG, "  Code length: %u", static_cast<unsigned>(code_.size()));
  ESP_LOGCONFIG(TAG, "  Shipments: %u", static_cast<unsigned>(shipments_));
}

void CameGatesSwitch::write_state(bool state) {
  if (state) {
    transmit_();
  }

  publish_state(state);
}

void CameGatesSwitch::send_bit_(uint8_t bit) {
  delayMicroseconds(BIT_TIME_US);
  if (bit == 0) {
    pin_->digital_write(true);
  }
  delayMicroseconds(BIT_TIME_US);
  pin_->digital_write(true);
  delayMicroseconds(BIT_TIME_US);
  pin_->digital_write(false);
}

bool CameGatesSwitch::build_transmission_code_(std::vector<uint8_t> *code) const {
  const size_t code_length = code_.size();
  if (code_length != 12 && code_length != 24) {
    ESP_LOGE(TAG, "Incorrect CAME code length: %u", static_cast<unsigned>(code_length));
    return false;
  }

  std::vector<uint8_t> raw_code;
  raw_code.reserve(code_length);
  for (char character : code_) {
    if (character != '0' && character != '1') {
      ESP_LOGE(TAG, "CAME code must contain only '0' and '1' characters");
      return false;
    }
    raw_code.push_back(character == '1' ? 1 : 0);
  }

  code->clear();
  code->reserve(code_length);

  if (code_length == 12) {
    for (int i = 5; i >= 0; i--) {
      code->push_back(raw_code[i]);
    }
    for (size_t i = 6; i < 12; i++) {
      code->push_back(raw_code[i]);
    }
    return true;
  }

  for (size_t block = 0; block < 3; block++) {
    for (int i = static_cast<int>((block + 1) * 8) - 1; i >= static_cast<int>(block * 8); i--) {
      code->push_back(raw_code[i]);
    }
  }

  return true;
}

void CameGatesSwitch::transmit_() {
  std::vector<uint8_t> transmission_code;
  if (!build_transmission_code_(&transmission_code)) {
    return;
  }

  for (uint32_t shipment = 0; shipment < shipments_; shipment++) {
    pin_->digital_write(true);
    delayMicroseconds(BIT_TIME_US);
    pin_->digital_write(false);

    for (uint8_t bit : transmission_code) {
      send_bit_(bit);
    }

    delay(16);
  }
}

}  // namespace came_gates_controller
}  // namespace esphome
