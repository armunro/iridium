#pragma once
#include <Arduino.h>
enum class EventType : uint8_t { INPUT_CHANGED, RELAY_REQUEST, RELAY_CHANGED, RELAY_REJECTED, RELAY_FAULT, STATE, SYSTEM };
struct PMCEvent {
 EventType type;
 uint32_t timestamp;
 uint8_t channel;
 bool active;
 char detail[48];
};
using EventHandler = void (*)(const PMCEvent&);
class EventBus {
public:
 bool subscribe(EventHandler handler);
 bool publish(EventType type, uint8_t channel, bool active, const char* detail);
 void dispatch(uint8_t limit=16);
 void printHistory() const;
 void printStats() const;
private:
 static constexpr uint8_t QUEUE_SIZE=24, HISTORY_SIZE=32, MAX_SUBSCRIBERS=6;
 PMCEvent queue_[QUEUE_SIZE]={}, history_[HISTORY_SIZE]={};
 EventHandler handlers_[MAX_SUBSCRIBERS]={};
 uint8_t head_=0, tail_=0, count_=0, historyNext_=0, historyCount_=0, subscribers_=0;
 uint32_t dropped_=0;
};
extern EventBus eventBus;
const char* eventTypeName(EventType type);
