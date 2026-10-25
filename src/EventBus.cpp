#include "EventBus.h"
#include <cstring>
EventBus eventBus;
const char* eventTypeName(EventType t) {
 switch(t) {
 case EventType::INPUT_CHANGED:return "INPUT"; case EventType::RELAY_REQUEST:return "RELAY_REQUEST";
 case EventType::RELAY_CHANGED:return "RELAY_CHANGED"; case EventType::RELAY_REJECTED:return "RELAY_REJECTED";
 case EventType::RELAY_FAULT:return "RELAY_FAULT"; case EventType::STATE:return "STATE";
 case EventType::SYSTEM:return "SYSTEM";
 } return "UNKNOWN";
}
bool EventBus::subscribe(EventHandler h) {
 if(!h || subscribers_>=MAX_SUBSCRIBERS)return false;
 handlers_[subscribers_++]=h; return true;
}
bool EventBus::publish(EventType t,uint8_t channel,bool active,const char* detail) {
 PMCEvent e={}; e.type=t;e.timestamp=millis();e.channel=channel;e.active=active;
 if(detail) { strncpy(e.detail,detail,sizeof(e.detail)-1);e.detail[sizeof(e.detail)-1]=0; }
 history_[historyNext_]=e;historyNext_=(historyNext_+1)%HISTORY_SIZE;
 if(historyCount_<HISTORY_SIZE)++historyCount_;
 if(count_>=QUEUE_SIZE) {++dropped_;return false;}
 queue_[tail_]=e;tail_=(tail_+1)%QUEUE_SIZE;++count_;return true;
}
void EventBus::dispatch(uint8_t limit) {
 while(count_ && limit--) {
  PMCEvent e=queue_[head_];head_=(head_+1)%QUEUE_SIZE;--count_;
  for(uint8_t i=0;i<subscribers_;++i) handlers_[i](e);
 }
}
void EventBus::printHistory() const {
 Serial.printf("[BUS] History entries=%u\n",historyCount_);
 for(uint8_t i=0;i<historyCount_;++i) {
  const auto& e=history_[(historyNext_+HISTORY_SIZE-historyCount_+i)%HISTORY_SIZE];
  Serial.printf("[BUS] %lu %s ch=%u active=%u %s\n",(unsigned long)e.timestamp,eventTypeName(e.type),e.channel,e.active,e.detail);
 }
}
void EventBus::printStats() const {Serial.printf("[BUS] queued=%u dropped=%lu subscribers=%u history=%u\n",count_,(unsigned long)dropped_,subscribers_,historyCount_);}
