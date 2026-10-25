#pragma once
#include <Arduino.h>
#include "ControlConfig.h"

class ControlManager {
 public:
  void begin();
  void setAvailabilityHandler(void (*handler)(uint8_t, bool)) { availabilityHandler_ = handler; }
  void setEventHandler(void (*handler)(const char*, bool, bool)) { handler_ = handler; }
  void update();
  void printMap() const;
  void printInputs() const;
  void printHealth() const;
  void setEventLogging(bool enabled) { eventLogging_ = enabled; }
  bool eventLogging() const { return eventLogging_; }
  bool connected(uint8_t address) const;
 private:
  struct Expander {
    uint8_t address = 0;
    bool online = false;
    uint8_t stable = 0xFF;
    uint8_t previous = 0xFF;
    uint32_t changedAt[8] = {};
    uint32_t lastPoll = 0;
    uint32_t lastProbe = 0;
  };
  Expander expanders_[8] = {};
  uint8_t expanderCount_ = 0;
  bool eventLogging_ = true;
  void (*handler_)(const char*, bool, bool) = nullptr;
  void (*availabilityHandler_)(uint8_t, bool) = nullptr;
  void syncSwitches(const Expander& device);
  bool probe(Expander& device);
  void poll(Expander& device, uint32_t now);
  bool read(uint8_t address, uint8_t& result);
  bool validate() const;
};
