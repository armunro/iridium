#include "CMCLink.h"
#include <HardwareSerial.h>
// UART2: ESP32 GPIO16=RX, GPIO17=TX. 3.3V TTL ONLY.
static HardwareSerial cmcSerial(2);
CMCLink cmcLink;
void CMCLink::begin() {
  cmcSerial.begin(115200, SERIAL_8N1, 16, 17);
  Serial.println("[CMC] UART2 initialized (GPIO16 RX / GPIO17 TX)");
}
void CMCLink::sendEvent(const char* label, const char* state) {
  cmcSerial.printf("EVENT|%s|%s\n", label, state);
  Serial.printf("[PMC->CMC] EVENT|%s|%s\n", label, state);
}
void CMCLink::handle(String line) {
  line.trim();
  if (line == "PONG") {
    if (!online_) Serial.println("[CMC] ONLINE");
    online_ = true;
    lastRx_ = millis();
  } else if (line.startsWith("ACK|")) {
    lastRx_ = millis();
    Serial.printf("[CMC] %s\n", line.c_str());
  } else if (line.startsWith("STATE|")) {
    lastRx_ = millis();
    Serial.printf("[CMC] %s (report only)\n", line.c_str());
  } else {
    Serial.printf("[CMC] Ignored: %s\n", line.c_str());
  }
}
void CMCLink::update() {
  while (cmcSerial.available()) {
    char c = (char)cmcSerial.read();
    if (c == '\n' || c == '\r') {
      if (buffer_.length()) { handle(buffer_); buffer_ = ""; }
    } else if (c >= 32 && c <= 126) {
      if (buffer_.length() < 120) buffer_ += c;
      else { buffer_ = ""; Serial.println("[CMC] Oversized line discarded"); }
    }
  }
  uint32_t now = millis();
  if (now - lastPing_ >= 2000) {
    lastPing_ = now;
    cmcSerial.print("PING\n");
  }
  if (online_ && now - lastRx_ > 6000) {
    online_ = false;
    Serial.println("[CMC] OFFLINE (heartbeat timeout)");
  }
}
void CMCLink::printStatus() const {
  Serial.printf("[CMC] %s\n", online_ ? "ONLINE" : "OFFLINE");
}
