#include "PMCStateManager.h"
#include "RelayManager.h"
#include "EventBus.h"
#include "ControlConfig.h"
#include <string.h>
PMCStateManager pmcState;
static const char* name(PMCStateManager::CmcPower p) {
  switch(p) {
    case PMCStateManager::CmcPower::OFF:return "OFF";
    case PMCStateManager::CmcPower::BOOT_REQUESTED:return "BOOT_REQUESTED";
    case PMCStateManager::CmcPower::RUNNING:return "RUNNING_SIMULATED";
    case PMCStateManager::CmcPower::SHUTDOWN_REQUESTED:return "SHUTDOWN_REQUESTED";
    case PMCStateManager::CmcPower::FAULT:return "FAULT";
  }
  return "UNKNOWN";
}
void PMCStateManager::begin() { cmc_=CmcPower::OFF; transitionAt_=millis(); peripherals_=caseLights_=panelLights_=false; caseKnown_=panelKnown_=false; }
void PMCStateManager::setCmc(CmcPower next,const char* reason) {
  if(cmc_==next)return;
  Serial.printf("[STATE] CMC %s -> %s (%s)\n",name(cmc_),name(next),reason);
  cmc_=next; transitionAt_=millis();
  eventBus.publish(EventType::STATE,0,false,reason);
}
void PMCStateManager::command(const String& cmd) {
  if(cmd=="POWER ON") {
    if(cmc_!=CmcPower::OFF) { Serial.println("[STATE] POWER ON rejected: not OFF");return; }
    if(relays.request(1,true,"CMC boot request"))setCmc(CmcPower::BOOT_REQUESTED,"POWER ON");
    else Serial.println("[STATE] CMC power not enabled in this release");
  } else if(cmd=="POWER SHUTDOWN") {
    if(cmc_!=CmcPower::RUNNING && cmc_!=CmcPower::BOOT_REQUESTED) { Serial.println("[STATE] SHUTDOWN rejected: not running/booting");return; }
    setCmc(CmcPower::SHUTDOWN_REQUESTED,"shutdown requested; R1 remains ON");
  } else if(cmd=="POWER SIM BOOTED") {
    if(cmc_==CmcPower::BOOT_REQUESTED) setCmc(CmcPower::RUNNING,"simulated boot confirmation");
    else Serial.println("[STATE] BOOTED rejected");
  } else if(cmd=="POWER SIM HALTED") {
    if(cmc_==CmcPower::SHUTDOWN_REQUESTED) { relays.request(1,false,"simulated verified halt");setCmc(CmcPower::OFF,"simulated halt"); }
    else Serial.println("[STATE] HALTED rejected");
  } else if(cmd=="POWER RESET FAULT") {
    if(cmc_==CmcPower::FAULT) { relays.request(1,false,"fault reset in simulation");setCmc(CmcPower::OFF,"manual reset"); }
    else Serial.println("[STATE] No fault to reset");
  } else Serial.println("[STATE] Unknown POWER command");
}
void PMCStateManager::onControl(const char* label,bool active,bool isButton) {
  if(isButton && !active)return;
  if(strncmp(label,"BTN1 ",5)==0) command("POWER ON");
  else if(strncmp(label,"BTN2 ",5)==0) command("POWER SHUTDOWN");
  else if(strncmp(label,"BTN3 ",5)==0 && isButton) {if(relays.request(4,!peripherals_,"BTN3 peripherals"))peripherals_=!peripherals_;}
  else if(strncmp(label,"SW2 ",4)==0) {
    caseKnown_=true;
    if(CASE_LIGHT_SWITCH_BEHAVIOR==SwitchBehavior::MAINTAINED) {
      caseLights_=active;
      relays.request(2,active,"SW2 maintained case lights");
    }
  }
  else if(strncmp(label,"SW3 ",4)==0) {
    panelKnown_=true;
    if(PANEL_LIGHT_SWITCH_BEHAVIOR==SwitchBehavior::MAINTAINED) {
      panelLights_=active;
      relays.request(3,active,"SW3 maintained panel lights");
    }
  }
  // PAL, AUX and network controls are reported, not actuated in this version.
}
void PMCStateManager::onInputAvailability(uint8_t address, bool available) {
  if (available) return; // Input manager publishes fresh positions on reconnect.
  for (size_t i=0; i<CONTROL_COUNT; ++i) {
    const auto& c=CONTROL_MAP[i];
    if(c.address!=address) continue;
    if(strncmp(c.label,"SW2 ",4)==0) {
      caseKnown_=false; caseLights_=false;
      relays.request(2,false,"SW2 input unavailable");
      eventBus.publish(EventType::SYSTEM,2,false,"SW2 position unknown");
    } else if(strncmp(c.label,"SW3 ",4)==0) {
      panelKnown_=false; panelLights_=false;
      relays.request(3,false,"SW3 input unavailable");
      eventBus.publish(EventType::SYSTEM,3,false,"SW3 position unknown");
    }
  }
}
void PMCStateManager::update() {
  // No guessed shutdown completion: R1 remains requested ON until verified halt.
  if(cmc_==CmcPower::BOOT_REQUESTED && millis()-transitionAt_>30000UL) setCmc(CmcPower::FAULT,"boot timeout; power unchanged");
  if(cmc_==CmcPower::SHUTDOWN_REQUESTED && millis()-transitionAt_>30000UL) setCmc(CmcPower::FAULT,"shutdown timeout; power unchanged");
}
void PMCStateManager::print() const {
  Serial.printf("[STATE] CMC=%s peripherals=%s caseLights=%s panelLights=%s\n",name(cmc_),peripherals_?"ON":"OFF",!caseKnown_?"UNKNOWN":caseLights_?"ON":"OFF",!panelKnown_?"UNKNOWN":panelLights_?"ON":"OFF");
  Serial.println("[STATE] PAL/AUX=SIMULATION ONLY | outputs=DISCONNECTED");
}
