# ESPHome CAME Gates

ESPHome external switch component for transmitting fixed-code CAME gate remote commands.

## Usage

```yaml
external_components:
  - source:
      type: git
      url: http://github.com/jehy/esphome-came-gates
      ref: main
      path: components
    components:
      - came_gates_controller

switch:
  - platform: came_gates_controller
    name: "Gates Switch"
    pin: D2
    code: "110010110110111111101111"
    shipments: 6
```

## Options

- `pin` - GPIO output pin connected to the transmitter data input.
- `code` - CAME fixed code. Must be exactly 12 or 24 bits and contain only `0` and `1`.
- `shipments` - Number of repeated transmissions. Defaults to `6`.
