#include "CMCLink.h"
#include "ControlManager.h"
#include <Wire.h>
#include <string.h>

bool ControlManager::read(uint8_t address, uint8_t& result) {
  if (Wire.requestFrom(address, static_cast<uint8_t>(1)) != 1) return false;
  result = Wire.read();
  return true;
}

bool ControlManager::validate() const {
  bool valid = true;
  for (size_t i = 0; i < CONTROL_COUNT; ++i) {
    const auto& a = CONTROL_MAP[i];
    if (a.address < 0x20 || a.address > 0x27 || a.pin > 7) {
      Serial.printf("[CONFIG] ERROR invalid address/pin: %s\n", a.label);
      valid = false;
    }
    for (size_t j = i+1; j < CONTROL_COUNT; ++j) {
      const auto& b = CONTROL_MAP[j];
      if (a.address == b.address && a.pin == b.pin) {
        Serial.printf("[CONFIG] ERROR duplicate 0x%02X/P%u: %s / %s\n",
                      a.address, a.pin, a.label, b.label);
        valid = false;
      }
    }
  }
  return valid;
}

bool ControlManager::probe(Expander& device) {
  Wire.beginTransmission(device.address);
  if (Wire.endTransmission() != 0) return false;
  // PCF8574 quasi-bidirectional: write HIGH on all pins to read inputs.
  Wire.beginTransmission(device.address);
  Wire.write(0xFF);
  if (Wire.endTransmission() != 0) return false;
  uint8_t sample;
  if (!read(device.address, sample)) return false;
  device.stable = sample;
  device.previous = sample;
  for (auto& t : device.changedAt) t = millis();
  device.online = true;
  Serial.printf("[IO] Connected PCF8574 at 0x%02X\n", device.address);
  return true;
}

void ControlManager::begin() {
  if (!validate()) {
    Serial.println("[CONFIG] INVALID MAP; input manager disabled");
    return;
  }
  // Register all configured expanders, including those with no mapped controls.
  bool addressesValid = EXPANDER_COUNT <= 8;
  for (size_t i = 0; i < EXPANDER_COUNT; ++i) {
    uint8_t address = EXPANDER_ADDRESSES[i];
    if (address < 0x20 || address > 0x27) {
      Serial.printf("[CONFIG] ERROR invalid expander address 0x%02X\n", address);
      addressesValid = false;
    }
    for (size_t j = 0; j < i; ++j) {
      if (EXPANDER_ADDRESSES[j] == address) {
        Serial.printf("[CONFIG] ERROR duplicate expander address 0x%02X\n", address);
        addressesValid = false;
      }
    }
  }
  for (size_t i = 0; i < CONTROL_COUNT; ++i) {
    bool found = false;
    for (size_t j = 0; j < EXPANDER_COUNT; ++j)
      if (CONTROL_MAP[i].address == EXPANDER_ADDRESSES[j]) found = true;
    if (!found) {
      Serial.printf("[CONFIG] ERROR %s references unregistered expander 0x%02X\n",
                    CONTROL_MAP[i].label, CONTROL_MAP[i].address);
      addressesValid = false;
    }
  }
  if (!addressesValid) {
    Serial.println("[CONFIG] INVALID EXPANDER LIST; input manager disabled");
    return;
  }
  for (size_t i = 0; i < EXPANDER_COUNT; ++i)
    expanders_[expanderCount_++].address = EXPANDER_ADDRESSES[i];
  for (uint8_t i = 0; i < expanderCount_; ++i) {
    auto& dev = expanders_[i];
    dev.lastProbe = millis();
    if (!probe(dev))
      Serial.printf("[IO] Missing configured PCF8574 at 0x%02X (safe/unavailable)\n", dev.address);
  }
}

bool ControlManager::connected(uint8_t address) const {
  for (uint8_t i = 0; i < expanderCount_; ++i)
    if (expanders_[i].address == address) return expanders_[i].online;
  return false;
}

void ControlManager::poll(Expander& dev, uint32_t now) {
  uint8_t sample;
  if (!read(dev.address, sample)) {
    dev.online = false;
    dev.lastProbe = now;
    Serial.printf("[IO] Lost 0x%02X; its inputs are now unavailable\n", dev.address);
    return;
  }
  for (uint8_t pin = 0; pin < 8; ++pin) {
    uint8_t mask = static_cast<uint8_t>(1U << pin);
    if ((sample & mask) != (dev.previous & mask)) dev.changedAt[pin] = now;
    if ((sample & mask) != (dev.stable & mask) &&
        now - dev.changedAt[pin] >= DEBOUNCE_MS) {
      dev.stable = (dev.stable & ~mask) | (sample & mask);
      bool mapped = false;
      for (size_t i = 0; i < CONTROL_COUNT; ++i) {
        const auto& c = CONTROL_MAP[i];
        if (c.address != dev.address || c.pin != pin) continue;
        mapped = true;
        bool active = !(dev.stable & mask);
        // Event routing is deliberately reporting-only in this bench firmware.
        cmcLink.sendEvent(c.label, c.type == ControlType::BUTTON ? (active ? "PRESSED" : "RELEASED") : (active ? "ON" : "OFF"));
        if (eventLogging_)
          Serial.printf("[EVENT] %s %s\n", c.label,
            c.type == ControlType::BUTTON
              ? (active ? "PRESSED" : "RELEASED")
              : (active ? "ON" : "OFF"));
      }
      if (!mapped && eventLogging_)
        Serial.printf("[INPUT] 0x%02X/P%u %s (UNMAPPED)\n",
                      dev.address, pin, (dev.stable & mask) ? "HIGH" : "LOW");
    }
  }
  dev.previous = sample;
}

void ControlManager::update() {
  uint32_t now = millis();
  for (uint8_t i = 0; i < expanderCount_; ++i) {
    auto& dev = expanders_[i];
    if (!dev.online) {
      if (now - dev.lastProbe >= REPROBE_MS) {
        dev.lastProbe = now;
        probe(dev);
      }
    } else if (now - dev.lastPoll >= POLL_MS) {
      dev.lastPoll = now;
      poll(dev, now);
    }
  }
}

void ControlManager::printMap() const {
  Serial.println("========== CONTROL MAP ==========");
  for (size_t i = 0; i < CONTROL_COUNT; ++i) {
    const auto& c = CONTROL_MAP[i];
    Serial.printf("0x%02X/P%u %-6s %-24s [%s]\n", c.address, c.pin,
      c.type == ControlType::BUTTON ? "BUTTON" : "SWITCH", c.label,
      connected(c.address) ? "ONLINE" : "MISSING");
  }
}

void ControlManager::printInputs() const {
  Serial.println("========== INPUT STATES ==========");
  for (size_t i = 0; i < CONTROL_COUNT; ++i) {
    const auto& c = CONTROL_MAP[i];
    const Expander* dev = nullptr;
    for (uint8_t j = 0; j < expanderCount_; ++j)
      if (expanders_[j].address == c.address) dev = &expanders_[j];
    const char* state = (!dev || !dev->online) ? "UNAVAILABLE" :
      (dev->stable & (1U << c.pin)) ? "INACTIVE" : "ACTIVE";
    Serial.printf("%-24s : %s\n", c.label, state);
  }
}

void ControlManager::printHealth() const {
  for (uint8_t i = 0; i < expanderCount_; ++i)
    Serial.printf("[IO] 0x%02X: %s\n", expanders_[i].address,
                  expanders_[i].online ? "ONLINE" : "MISSING");
}
