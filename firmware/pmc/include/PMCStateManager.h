#pragma once
#include <Arduino.h>
class PMCStateManager {
public:
  enum class CmcPower : uint8_t { OFF, BOOT_REQUESTED, RUNNING, SHUTDOWN_REQUESTED, FAULT };
  void begin();
  void update();
  void onControl(const char* label, bool active, bool isButton);
  void onInputAvailability(uint8_t address, bool available);
  void command(const String& cmd);
  void print() const;
private:
  CmcPower cmc_ = CmcPower::OFF;
  bool peripherals_ = false, caseLights_ = false, panelLights_ = false;
  bool caseKnown_ = false, panelKnown_ = false;
  uint32_t transitionAt_ = 0;
  void setCmc(CmcPower next, const char* reason);
};
extern PMCStateManager pmcState;
