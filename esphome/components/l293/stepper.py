import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import stepper
from esphome.const import CONF_ID, CONF_PIN_A, CONF_PIN_B, CONF_PIN_EN1, CONF_PIN_EN2, \
    CONF_SLEEP_WHEN_DONE, CONF_STEP_MODE

l293_ns = cg.esphome_ns.namespace('l293')
L293StepMode = l293_ns.enum('L293StepMode')

STEP_MODES = {
    'FULL_STEP': L293StepMode.L293_STEP_MODE_FULL_STEP,
    'HALF_STEP': L293StepMode.L293_STEP_MODE_HALF_STEP,
    'WAVE_DRIVE': L293StepMode.L293_STEP_MODE_WAVE_DRIVE,
}

L293 = l293_ns.class_('L293', stepper.Stepper, cg.Component)

CONFIG_SCHEMA = stepper.STEPPER_SCHEMA.extend({
    cv.Required(CONF_ID): cv.declare_id(L293),
    cv.Required(CONF_PIN_A): pins.gpio_output_pin_schema,
    cv.Required(CONF_PIN_B): pins.gpio_output_pin_schema,
    cv.Required(CONF_PIN_EN1): pins.gpio_output_pin_schema,
    cv.Required(CONF_PIN_EN2): pins.gpio_output_pin_schema,
    cv.Optional(CONF_SLEEP_WHEN_DONE, default=True): cv.boolean,
    cv.Optional(CONF_STEP_MODE, default='FULL_STEP'): cv.enum(STEP_MODES, upper=True, space='_')
}).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield stepper.register_stepper(var, config)

    pin_a = yield cg.gpio_pin_expression(config[CONF_PIN_A])
    cg.add(var.set_pin_a(pin_a))
    pin_b = yield cg.gpio_pin_expression(config[CONF_PIN_B])
    cg.add(var.set_pin_b(pin_b))
    pin_en1 = yield cg.gpio_pin_expression(config[CONF_PIN_EN1])
    cg.add(var.set_pin_en1(pin_en1))
    pin_en2 = yield cg.gpio_pin_expression(config[CONF_PIN_EN2])
    cg.add(var.set_pin_en2(pin_en2))

    cg.add(var.set_sleep_when_done(config[CONF_SLEEP_WHEN_DONE]))
    cg.add(var.set_step_mode(config[CONF_STEP_MODE]))
