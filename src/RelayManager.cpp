#include "RelayManager.h"
#include "EventBus.h"
#include <Wire.h>
RelayManager relays;
uint8_t RelayManager::byteForDesired() const {
 uint8_t value=offByte();
 for(uint8_t i=0;i<8;++i) if(desired_[i]) {
  if(RELAY_TRIGGER==RelayPolarity::ACTIVE_LOW)value &= ~(1u<<i);
  else value |= (1u<<i);
 }
 return value;
}
bool RelayManager::writeByte(uint8_t value) {
 Wire.beginTransmission(RELAY_I2C_ADDRESS);Wire.write(value);
 if(Wire.endTransmission()!=0)return false;
 shadow_=value;return true;
}
void RelayManager::fault(const char* reason) {
 online_=false; initialized_=false;
 for(bool &d:desired_)d=false;
 eventBus.publish(EventType::RELAY_FAULT,0,false,reason);
 Serial.printf("[RELAY] FAULT %s; physical state cannot be verified\n",reason);
}
bool RelayManager::probe() {
 Wire.beginTransmission(RELAY_I2C_ADDRESS);
 if(Wire.endTransmission()!=0)return false;
 // Explicit inactive byte before allowing any ON request.
 if(RELAY_HARDWARE_OUTPUTS_ENABLED && !writeByte(offByte()))return false;
 for(bool &d:desired_)d=false;
 online_=true;initialized_=true;
 eventBus.publish(EventType::SYSTEM,0,false,"Relay expander detected; outputs reset OFF");
 Serial.printf("[RELAY] IO5 0x%02X online; hardware=%s\n",RELAY_I2C_ADDRESS,RELAY_HARDWARE_OUTPUTS_ENABLED?"ENABLED":"DISABLED");
 return true;
}
void RelayManager::begin() {
 valid_=(RELAY_I2C_ADDRESS>=0x20 && RELAY_I2C_ADDRESS<=0x27);
 shadow_=offByte();lastProbe_=millis();
 if(!valid_){fault("invalid IO5 address");return;}
 if(!probe())fault("IO5 missing or initialization failed");
}
void RelayManager::update() {
 uint32_t now=millis();
 if(!online_ && now-lastProbe_>=RELAY_REPROBE_MS){lastProbe_=now;if(!probe())return;}
 // No I2C readback can verify physical relay contacts.
}
bool RelayManager::request(uint8_t relay,bool on,const char* reason) {
 eventBus.publish(EventType::RELAY_REQUEST,relay,on,reason);
 if(relay<1 || relay>8 || !valid_ || !online_ || !initialized_) {
  eventBus.publish(EventType::RELAY_REJECTED,relay,on,"Invalid channel or IO5 unavailable");return false;
 }
 if(on && !(RELAY_ALLOWED_ON_MASK & (1u<<(relay-1)))) {
  eventBus.publish(EventType::RELAY_REJECTED,relay,on,"Channel not permitted to energize");return false;
 }
 if(on && !RELAY_HARDWARE_OUTPUTS_ENABLED) {
  eventBus.publish(EventType::RELAY_REJECTED,relay,on,"Hardware outputs disabled");return false;
 }
 bool prior=desired_[relay-1];desired_[relay-1]=on;
 if(RELAY_HARDWARE_OUTPUTS_ENABLED && !writeByte(byteForDesired())) {
  fault("I2C output write failed");return false;
 }
 if(prior!=on)eventBus.publish(EventType::RELAY_CHANGED,relay,on,reason);
 return true;
}
bool RelayManager::isOn(uint8_t relay) const {return relay>=1 && relay<=8 && desired_[relay-1];}
void RelayManager::allOff(const char* reason) {
 for(bool &d:desired_)d=false;
 if(RELAY_HARDWARE_OUTPUTS_ENABLED && online_ && !writeByte(offByte()))fault("allOff write failed");
 eventBus.publish(EventType::RELAY_CHANGED,0,false,reason);
}
void RelayManager::print() const {
 Serial.printf("[RELAY] IO5=0x%02X online=%s hardware=%s shadow=0x%02X (not contact feedback)\n",RELAY_I2C_ADDRESS,online_?"YES":"NO",RELAY_HARDWARE_OUTPUTS_ENABLED?"ENABLED":"DISABLED",shadow_);
 for(uint8_t i=0;i<8;++i)Serial.printf("[RELAY] R%u P%u %-17s requested=%s allowedON=%s\n",i+1,i,RELAY_NAMES[i],desired_[i]?"ON":"OFF",(RELAY_ALLOWED_ON_MASK&(1u<<i))?"YES":"NO");
}
void RelayManager::printConfig() const {
 Serial.printf("[RELAY] Global trigger=%s; IO5=%02X; enabled=%u; ON mask=%02X\n",RELAY_TRIGGER==RelayPolarity::ACTIVE_LOW?"LOW":"HIGH",RELAY_I2C_ADDRESS,RELAY_HARDWARE_OUTPUTS_ENABLED,RELAY_ALLOWED_ON_MASK);
 print();
}
