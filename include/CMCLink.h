#pragma once
#include <Arduino.h>
class CMCLink {
 public:
  void begin();
  void update();
  void sendEvent(const char* label, const char* state);
  void printStatus() const;
 private:
  String buffer_;
  uint32_t lastPing_ = 0, lastRx_ = 0;
  bool online_ = false;
  void handle(String line);
};
extern CMCLink cmcLink;
