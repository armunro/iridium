#pragma once
#include <Arduino.h>
// Dedicated PCF8574 IO5. Configure this address to match the hardware scan.
constexpr uint8_t RELAY_I2C_ADDRESS = 0x24;
enum class RelayPolarity : uint8_t { ACTIVE_LOW, ACTIVE_HIGH };
// Global selector on the eight-channel relay board; verify electrically.
constexpr RelayPolarity RELAY_TRIGGER = RelayPolarity::ACTIVE_LOW;
// Deliberately OFF until input-current compatibility and reset behavior are verified.
// Enabling this writes the inactive byte to the real expander and permits only R2/R3 ON.
constexpr bool RELAY_HARDWARE_OUTPUTS_ENABLED = false;
constexpr uint8_t RELAY_ALLOWED_ON_MASK = (1u << 1) | (1u << 2);
constexpr uint32_t RELAY_REPROBE_MS = 2000;
constexpr const char* RELAY_NAMES[8] = {
 "CMC power", "Case lights", "Panel lights", "USB peripherals",
 "PAL interlock", "AUX trigger", "Reserved 7", "Reserved 8"
};
