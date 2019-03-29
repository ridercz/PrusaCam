/* Altairis PrusaCam Feather BT version 1.0.0 (2019-03-29)
   Copyright (c) Michal A. Valasek - Altairis, 2019
   Licensed under terms of the MIT License.
   www.rider.cz | www.altairis.cz | github.com/ridercz/PrusaCam
*/

#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "PrusaCamConfig.h"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
bool serialAvailable = false;
unsigned long nextShutterMin = 0;

#ifdef USE_INTERVAL
unsigned long nextShutterMillis = 0;
#endif

#ifdef USE_TRIGGER
bool lastTriggerState = false;
#endif

void setup() {
  // Turn on LED during initialization
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, true);

  // Initialize serial port
  Serial.begin(9600);
  while (true) {
    if (Serial) {
      serialAvailable = true;
      break;
    }
    if (millis() > INITIAL_DELAY) break;
  }

  logPrintLn(F("Altairis PrusaCam Feather BT version 1.0.0"));
  logPrintLn(F("Copyright (c) Michal A. Valasek - Altairis, 2019"));
  logPrintLn(F("              www.rider.cz | www.altairis.cz | github.com/ridercz/PrusaCam"));
  logPrintLn();

  // Initialize Bluetooth
  initBLE();

  // Check if there is either interval or trigger
  bool hasSomethigToRunOn = false;

#ifdef USE_TRIGGER
  logPrint(F("Trigger enabled on pin "));
  logPrintLn(TRIGGER_PIN);
  pinMode(TRIGGER_PIN, INPUT);
  if (TRIGGER_ON_LH || TRIGGER_ON_HL) {
    if (TRIGGER_ON_LH) logPrintLn(F("Trigger on LH enabled."));
    if (TRIGGER_ON_HL) logPrintLn(F("Trigger on HL enabled."));
    hasSomethigToRunOn = true;
  } else {
    logPrintLn(F("WARNING: Neither TRIGGER_ON_LH nor TRIGGER_ON_HL is set to true. Trigger is effectively disabled."));
  }
#endif

#ifdef USE_INTERVAL
  logPrint(F("Inteval enabled, time (ms) = "));
  logPrintLn(INTERVAL_SHUTTER);
  hasSomethigToRunOn = true;
#endif

  if (!hasSomethigToRunOn) {
    logPrintLn(F("CONFIGURATION ERROR: There are no conditions for shutter defined, so the shutter will never be pressed."));
    logPrintLn(F("System halted."));
    while (1);
  }

  // Turn off LED
  logPrintLn(F("Initialization done, system ready."));
  digitalWrite(LED_BUILTIN, false);
}

void loop() {
#ifdef USE_TRIGGER
  bool currentTriggerState = digitalRead(TRIGGER_PIN);
  if (lastTriggerState != currentTriggerState) {
    if (TRIGGER_ON_LH && currentTriggerState) {
      logPrint("Trigger (LH) detected: ");
      pressShutter();
    }
    if (TRIGGER_ON_HL && !currentTriggerState) {
      logPrint("Trigger (HL) detected: ");
      pressShutter();
    }
    lastTriggerState = currentTriggerState;
  }
#endif

#ifdef USE_INTERVAL
  // Press shutter every INTERVAL_SHUTTER ms
  if (millis() >= nextShutterMillis) {
    pressShutter();
    logPrintLn(F("Waiting..."));
    nextShutterMillis = millis() + INTERVAL_SHUTTER;
  }
#endif
}

void initBLE() {
  logPrint(F("Initializing the Bluetooth module..."));
  if (!ble.begin(BLUEFRUIT_VERBOSE_MODE)) {
    logPrintLn(F("Failed!"));
    while (1);
  }
  logPrintLn(F("OK"));

  if (serialAvailable) {
    // Disable command echo
    ble.echo(false);
    // Display module info
    ble.info();
  }

#ifdef PERFORM_FACTORY_RESET
  logPrint(F("Performing a factory reset..."));
  if (!ble.factoryReset()) {
    logPrintLn(F("Failed!"));
    while (1);
  }
  logPrintLn(F("OK"));
#endif

  // Firmware version 0.6.6+ required
  if (!ble.isVersionAtLeast(BLUEFRUIT_MINIMUM_FIRMWARE_VERSION)) {
    logPrintLn(F("This sketch requires firmware version "BLUEFRUIT_MINIMUM_FIRMWARE_VERSION" or higher!"));
    while (1);
  }

  logPrint(F("Setting device name to 'Altairis PrusaCam Feather BT'..."));
  if (!ble.sendCommandCheckOK(F("AT+GAPDEVNAME=Altairis PrusaCam Feather BT"))) {
    logPrintLn(F("Failed!"));
    while (1);
  }
  logPrintLn(F("OK"));

  logPrint(F("Enabling HID Services..."));
  if (!ble.sendCommandCheckOK(F("AT+BLEHIDEN=On"))) {
    logPrintLn(F("Failed!"));
    while (1);
  }
  logPrintLn(F("OK"));

  logPrint(F("Applying changes..."));
  if (!ble.reset()) {
    logPrintLn(F("Failed!"));
    while (1);
  }
  logPrintLn(F("OK"));
}

void pressShutter() {
  if (millis() < nextShutterMin) {
    logPrintLn(F("Ignoring shutter request, too early after previous one."));
    return;
  }

  // Turn on LED during shutter press
  digitalWrite(LED_BUILTIN, true);

  // Send VOLUME+
  logPrint(F("Pressing shutter..."));
  ble.println(F("AT+BleHidControlKey=VOLUME+"));
  if (!ble.waitForOK()) {
    logPrintLn(F("Failed! (maybe not paired?)"));
  } else {
    logPrintLn(F("OK"));
  }

  // Turn off LED
  digitalWrite(LED_BUILTIN, false);

  // Set minimum time for next shutter
  nextShutterMin = millis() + SHUTTER_MIN_INTERVAL;
}

void logPrintLn() {
  if (!serialAvailable) return;
  Serial.println();
}

void logPrintLn(char* s) {
  if (!serialAvailable) return;
  Serial.println(s);
}

void logPrintLn(String s) {
  if (!serialAvailable) return;
  Serial.println(s);
}

void logPrint(String s) {
  if (!serialAvailable) return;
  Serial.print(s);
}

void logPrint(char* s) {
  if (!serialAvailable) return;
  Serial.print(s);
}
