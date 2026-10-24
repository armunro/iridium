#pragma once
#include <Arduino.h>

// Change ONLY this table to reassign controls. Each entry: {address, pin, type, label}.
// Inputs are active-low: pressed/ON = connected to GND.
// Standard PCF8574 address range: 0x20..0x27.
// All five input expanders are explicitly registered, even if pins are unmapped.
// EDIT these addresses to match your I2C SCAN results; 0x20..0x24 are defaults.
// IMPORTANT: All registered PCF8574 pins are initialized HIGH (input mode).
// Do not connect output loads to these pins with this input-only firmware.
constexpr uint8_t EXPANDER_ADDRESSES[] = {0x20, 0x21, 0x22, 0x23, 0x24};
constexpr size_t EXPANDER_COUNT = sizeof(EXPANDER_ADDRESSES) / sizeof(EXPANDER_ADDRESSES[0]);
enum class ControlType : uint8_t { SWITCH, BUTTON };
struct ControlDefinition {
  uint8_t address;
  uint8_t pin;
  ControlType type;
  const char* label;
};
constexpr ControlDefinition CONTROL_MAP[] = {
  {0x20, 0, ControlType::SWITCH, "SW2 CASE_LIGHTS"},
  {0x20, 1, ControlType::SWITCH, "SW3 PANEL_LIGHTS"},
  {0x20, 2, ControlType::SWITCH, "SW4 AUX_MAIN"},
  {0x20, 3, ControlType::SWITCH, "SW5 AUX_SOURCE"},
  {0x20, 4, ControlType::SWITCH, "SW6 AUX_ARM"},
  {0x20, 5, ControlType::SWITCH, "SW7 PAL_BYPASS"},
  {0x21, 0, ControlType::BUTTON, "BTN1 CMC_POWER_ON"},
  {0x21, 1, ControlType::BUTTON, "BTN2 CMC_SHUTDOWN"},
  {0x21, 2, ControlType::BUTTON, "BTN3 PERIPHERALS"},
  {0x21, 3, ControlType::BUTTON, "BTN4 ETHERNET"},
  {0x21, 4, ControlType::BUTTON, "BTN5 WIFI"},
  {0x21, 5, ControlType::BUTTON, "BTN6 SSH"},
  {0x21, 6, ControlType::BUTTON, "BTN7 TRIGGER_MODE"},
  {0x21, 7, ControlType::BUTTON, "BTN8 FIRE"},
  {0x22, 0, ControlType::BUTTON, "BTN9 AUTOMATION"},
  {0x22, 1, ControlType::BUTTON, "BTN10 START"},
  {0x22, 2, ControlType::BUTTON, "BTN11 PAUSE"},
  {0x22, 3, ControlType::BUTTON, "BTN12 STOP"}
};
constexpr size_t CONTROL_COUNT = sizeof(CONTROL_MAP)/sizeof(CONTROL_MAP[0]);
constexpr uint8_t I2C_SDA_PIN = 21;
constexpr uint8_t I2C_SCL_PIN = 22;
constexpr uint32_t DEBOUNCE_MS = 40;
constexpr uint32_t POLL_MS = 20;
constexpr uint32_t REPROBE_MS = 2000;
