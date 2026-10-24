#include "CMCLink.h"
#include <Arduino.h>
#include <Wire.h>
#include "ControlManager.h"

// PMC v0.5: bench-test ONLY. NO relay, AUX, mains or PAL hardware actuation.
constexpr uint32_t HEARTBEAT_MS = 5000;
constexpr uint32_t PAL_AUTH_MS = 10000;
constexpr size_t MAX_COMMAND = 128;
ControlManager controls;
enum class PalState { LOCKED, AUTHORIZED, FAULT };
enum class AutoState { IDLE, RUNNING, PAUSED };
PalState pal = PalState::LOCKED;
AutoState automation = AutoState::IDLE;
bool heartbeatLogging = false;
uint32_t heartbeatCount = 0, lastHeartbeat = 0, palStarted = 0;
String commandBuffer;

const char* palName() {
  switch (pal) {
    case PalState::LOCKED: return "LOCKED";
    case PalState::AUTHORIZED: return "AUTHORIZED (SIMULATED)";
    case PalState::FAULT: return "FAULT (SIMULATED)";
  }
  return "UNKNOWN";
}
const char* autoName() {
  switch (automation) {
    case AutoState::IDLE: return "IDLE";
    case AutoState::RUNNING: return "RUNNING";
    case AutoState::PAUSED: return "PAUSED";
  }
  return "UNKNOWN";
}
void scanI2C() {
  Serial.println("[I2C] Scanning...");
  uint8_t count = 0;
  for (uint8_t a = 1; a < 127; ++a) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) {
      Serial.printf("[I2C] Found 0x%02X\n", a);
      ++count;
    }
  }
  Serial.printf("[I2C] %u device(s) found\n", count);
}
void printHelp() {
  Serial.println("HELP STATUS PING MAP INPUTS I2C SCAN CMC STATUS");
  Serial.println("EVENTS ON | EVENTS OFF");
  Serial.println("HEARTBEAT ON | HEARTBEAT OFF");
  Serial.println("PAL AUTH | PAL LOCK | PAL FAULT | PAL RESET");
  Serial.println("AUTO START | AUTO PAUSE | AUTO STOP");
  Serial.println("PAL and AUTO commands are simulations; no hardware outputs.");
}
void printStatus() {
  Serial.printf("[PMC] ONLINE | uptime=%lu s | heartbeats=%lu | heartbeat log=%s | event log=%s\n",
    static_cast<unsigned long>(millis()/1000UL),
    static_cast<unsigned long>(heartbeatCount),
    heartbeatLogging ? "ON" : "OFF", controls.eventLogging() ? "ON" : "OFF");
  Serial.printf("[PAL] %s\n", palName());
  if (pal == PalState::AUTHORIZED) {
    uint32_t elapsed = millis() - palStarted;
    Serial.printf("[PAL] Remaining=%lu ms\n",
      static_cast<unsigned long>(elapsed < PAL_AUTH_MS ? PAL_AUTH_MS - elapsed : 0));
  }
  Serial.printf("[AUTO] %s\n", autoName());
  controls.printHealth();
  cmcLink.printStatus();
  Serial.println("[PMC] ALL PHYSICAL OUTPUTS DISABLED");
}
void processCommand(String command) {
  command.trim();
  command.toUpperCase();
  if (command.length() == 0) return;
  if (command == "HELP") printHelp();
  else if (command == "STATUS") printStatus();
  else if (command == "PING") Serial.println("[PMC] PONG");
  else if (command == "CMC STATUS") cmcLink.printStatus();
  else if (command == "MAP") controls.printMap();
  else if (command == "INPUTS") controls.printInputs();
  else if (command == "I2C SCAN") scanI2C();
  else if (command == "EVENTS ON") { controls.setEventLogging(true); Serial.println("[PMC] Event logging ON"); }
  else if (command == "EVENTS OFF") { controls.setEventLogging(false); Serial.println("[PMC] Event logging OFF"); }
  else if (command == "HEARTBEAT ON") { heartbeatLogging = true; Serial.println("[PMC] Heartbeat logging ON"); }
  else if (command == "HEARTBEAT OFF") { heartbeatLogging = false; Serial.println("[PMC] Heartbeat logging OFF"); }
  else if (command == "PAL AUTH") {
    if (pal == PalState::FAULT) Serial.println("[PAL] Rejected: fault active");
    else { pal = PalState::AUTHORIZED; palStarted = millis(); Serial.println("[PAL] Simulated authorization: 10 seconds"); }
  }
  else if (command == "PAL LOCK") { pal = PalState::LOCKED; Serial.println("[PAL] LOCKED"); }
  else if (command == "PAL FAULT") { pal = PalState::FAULT; Serial.println("[PAL] Simulated FAULT"); }
  else if (command == "PAL RESET") { pal = PalState::LOCKED; Serial.println("[PAL] Reset to LOCKED"); }
  else if (command == "AUTO START") {
    if (automation == AutoState::RUNNING) Serial.println("[AUTO] Already running");
    else { automation = AutoState::RUNNING; Serial.println("[AUTO] Started/resumed (simulated)"); }
  }
  else if (command == "AUTO PAUSE") {
    if (automation != AutoState::RUNNING) Serial.println("[AUTO] Not running");
    else { automation = AutoState::PAUSED; Serial.println("[AUTO] Paused (simulated)"); }
  }
  else if (command == "AUTO STOP") { automation = AutoState::IDLE; Serial.println("[AUTO] Stopped (simulated)"); }
  else { Serial.printf("[PMC] Unknown command: %s\n", command.c_str()); }
}
void updateSerial() {
  while (Serial.available()) {
    char ch = static_cast<char>(Serial.read());
    if (ch == '\r' || ch == '\n') {
      if (commandBuffer.length()) { processCommand(commandBuffer); commandBuffer = ""; }
    } else if (ch == '\b' || ch == 127) {
      if (commandBuffer.length()) commandBuffer.remove(commandBuffer.length()-1);
    } else if (ch >= 32 && ch <= 126) {
      if (commandBuffer.length() < MAX_COMMAND) commandBuffer += ch;
      else { commandBuffer = ""; Serial.println("[PMC] Command too long"); }
    }
  }
}
void setup() {
  Serial.begin(115200);
  delay(600);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(100000);
  Serial.println("\nPMC v0.6 | PCF8574 Control Manager | BENCH MODE");
  cmcLink.begin();
  controls.begin();
  Serial.println("Type HELP. No physical outputs are enabled.");
  lastHeartbeat = millis();
}
void loop() {
  updateSerial();
  controls.update();
  cmcLink.update();
  uint32_t now = millis();
  if (now - lastHeartbeat >= HEARTBEAT_MS) {
    lastHeartbeat = now;
    ++heartbeatCount;
    if (heartbeatLogging)
      Serial.printf("[PMC] HEARTBEAT #%lu\n", static_cast<unsigned long>(heartbeatCount));
  }
  if (pal == PalState::AUTHORIZED && now - palStarted >= PAL_AUTH_MS) {
    pal = PalState::LOCKED;
    Serial.println("[PAL] Simulated authorization expired");
  }
  delay(1);
}
