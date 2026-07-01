import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import switch
from esphome.const import CONF_ID, CONF_PIN

CONF_CODE = "code"
CONF_SHIPMENTS = "shipments"

came_gates_controller_ns = cg.esphome_ns.namespace("came_gates_controller")
CameGatesSwitch = came_gates_controller_ns.class_(
    "CameGatesSwitch", switch.Switch, cg.Component
)


def validate_came_code(value):
    value = cv.string_strict(value)
    if len(value) not in (12, 24):
        raise cv.Invalid("CAME code must be exactly 12 or 24 bits long")
    if any(character not in "01" for character in value):
        raise cv.Invalid("CAME code must contain only '0' and '1' characters")
    return value


CONFIG_SCHEMA = switch.switch_schema(CameGatesSwitch).extend(
    {
        cv.Required(CONF_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_CODE): validate_came_code,
        cv.Optional(CONF_SHIPMENTS, default=6): cv.positive_int,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = await switch.new_switch(config)
    await cg.register_component(var, config)

    pin = await cg.gpio_pin_expression(config[CONF_PIN])
    cg.add(var.set_pin(pin))
    cg.add(var.set_code(config[CONF_CODE]))
    cg.add(var.set_shipments(config[CONF_SHIPMENTS]))
