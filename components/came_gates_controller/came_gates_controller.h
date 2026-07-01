#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"
#include "esphome/core/gpio.h"

namespace esphome {
namespace came_gates_controller {

class CameGatesSwitch : public switch_::Switch, public Component {
 public:
  void set_pin(GPIOPin *pin) { pin_ = pin; }
  void set_code(const std::string &code) { code_ = code; }
  void set_shipments(uint32_t shipments) { shipments_ = shipments; }

  void setup() override;
  void dump_config() override;
  void write_state(bool state) override;

 protected:
  static constexpr uint32_t BIT_TIME_US = 320;

  void send_bit_(uint8_t bit);
  bool build_transmission_code_(std::vector<uint8_t> *code) const;
  void transmit_();

  GPIOPin *pin_{nullptr};
  std::string code_;
  uint32_t shipments_{6};
};

}  // namespace came_gates_controller
}  // namespace esphome
