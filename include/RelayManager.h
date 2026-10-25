#pragma once
#include <Arduino.h>
#include "RelayConfig.h"
class RelayManager {
public:
 void begin(); void update();
 bool request(uint8_t relay,bool on,const char* reason);
 bool isOn(uint8_t relay) const;
 void allOff(const char* reason);
 void print() const; void printConfig() const;
 bool online() const {return online_;}
private:
 bool desired_[8]={};
 bool online_=false, valid_=false, initialized_=false;
 uint8_t shadow_=0xFF;
 uint32_t lastProbe_=0;
 uint8_t offByte() const {return RELAY_TRIGGER==RelayPolarity::ACTIVE_LOW?0xFF:0x00;}
 uint8_t byteForDesired() const;
 bool writeByte(uint8_t value);
 bool probe();
 void fault(const char* reason);
};
extern RelayManager relays;
