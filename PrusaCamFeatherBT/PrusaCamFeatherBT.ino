/* Altairis PrusaCam Feather BT version 1.0.0 (2019-02-28)
 * Copyright (c) Michal A. Valasek - Altairis, 2019 
 * Licensed under terms of the MIT License.
 * www.rider.cz | www.altairis.cz | github.com/ridercz/PrusaCam
 */
  
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include <Pushbutton.h>
#include "PrusaCamConfig.h"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

#ifdef USE_INTERVAL
unsigned long nextDotMillis = 0;
unsigned long nextShutterMillis = 0;
#endif

#ifdef USE_TRIGGER
Pushbutton trigger(TRIGGER_PIN);
#endif

void setup() {
  // Turn on LED during initialization
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, true);

  // Initialize
  while (!Serial);
  Serial.begin(9600);
  delay(INITIAL_DELAY);
  Serial.println(F("Altairis PrusaCam Feather BT version 1.0.0"));
  Serial.println(F("Copyright (c) Michal A. Valasek - Altairis, 2019"));
  Serial.println(F("              www.rider.cz | www.altairis.cz | github.com/ridercz/PrusaCam"));
  Serial.println();
  initBLE();

  // Check if there is either interval or trigger
  bool hasSomethigToRunOn = false;
  
#ifdef USE_TRIGGER
  Serial.print(F("Trigger enabled on pin "));
  Serial.println(TRIGGER_PIN);
  if (TRIGGER_ON_PRESS || TRIGGER_ON_RELEASE) {
    if (TRIGGER_ON_PRESS) Serial.println(F("Trigger on press enabled."));
    if (TRIGGER_ON_RELEASE) Serial.println(F("Trigger on release enabled."));
    hasSomethigToRunOn = true;
  } else {
    Serial.println(F("WARNING: Neither TRIGGER_ON_PRESS nor TRIGGER_ON_RELEASE is set to true. Trigger is effectively disabled."));
  }
#endif

#ifdef USE_INTERVAL
  Serial.print(F("Inteval enabled, time (ms) = "));
  Serial.println(INTERVAL_SHUTTER);
  hasSomethigToRunOn = true;
#endif

  if (!hasSomethigToRunOn) {
    Serial.println(F("CONFIGURATION ERROR: There are no conditions for shutter defined, so the shutter will never be pressed."));
    Serial.println(F("System halted."));
    while (1);
  }

  // Turn off LED
  Serial.println(F("Initialization done, system ready."));
  digitalWrite(LED_BUILTIN, false);
}

void loop() {
#ifdef USE_TRIGGER
  if (TRIGGER_ON_PRESS && trigger.getSingleDebouncedPress()) {
    Serial.println();
    Serial.println(F("Trigger pin pressed!"));
    pressShutter();
  }
  if (TRIGGER_ON_RELEASE && trigger.getSingleDebouncedRelease()) {
    Serial.println();
    Serial.println(F("Trigger pin released!"));
    pressShutter();
  }
#endif

#ifdef USE_INTERVAL
  // Press shutter every INTERVAL_SHUTTER ms
  if (millis() >= nextShutterMillis) {
    Serial.println();

    pressShutter();

    Serial.print(F("Waiting"));
    nextShutterMillis = millis() + INTERVAL_SHUTTER;
    nextDotMillis = millis() + INTERVAL_DOT;
  }

  // Print dot every INTERVAL_DOT ms to show we're alive
  if (millis() >= nextDotMillis) {
    Serial.print(".");
    nextDotMillis = millis() + INTERVAL_DOT;
  }
#endif
}

void initBLE() {
  Serial.print(F("Initializing the Bluetooth module..."));
  if (!ble.begin(BLUEFRUIT_VERBOSE_MODE)) {
    Serial.println(F("Failed!"));
    while (1);
  }
  Serial.println(F("OK"));

  // Disable command echo
  ble.echo(false);

  // Display module info
  ble.info();

#ifdef PERFORM_FACTORY_RESET
  Serial.print(F("Performing a factory reset..."));
  if (!ble.factoryReset()) {
    Serial.println(F("Failed!"));
    while (1);
  }
  Serial.println(F("OK"));
#endif

  // Firmware version 0.6.6+ required
  if (!ble.isVersionAtLeast(BLUEFRUIT_MINIMUM_FIRMWARE_VERSION)) {
    Serial.println(F("This sketch requires firmware version "BLUEFRUIT_MINIMUM_FIRMWARE_VERSION" or higher!"));
    while (1);
  }

  Serial.print(F("Setting device name to 'Altairis PrusaCam Feather BT'..."));
  if (!ble.sendCommandCheckOK(F("AT+GAPDEVNAME=Altairis PrusaCam Feather BT"))) {
    Serial.println(F("Failed!"));
    while (1);
  }
  Serial.println(F("OK"));

  Serial.print(F("Enabling HID Services..."));
  if (!ble.sendCommandCheckOK(F("AT+BLEHIDEN=On"))) {
    Serial.println(F("Failed!"));
    while (1);
  }
  Serial.println(F("OK"));

  Serial.print(F("Applying changes..."));
  if (!ble.reset()) {
    Serial.println(F("Failed!"));
    while (1);
  }
  Serial.println(F("OK"));
}

void pressShutter() {
  // Turn on LED during shutter press
  digitalWrite(LED_BUILTIN, true);

  // Send VOLUME+
  Serial.print(F("Pressing shutter..."));
  ble.println(F("AT+BleHidControlKey=VOLUME+"));
  if (!ble.waitForOK()) {
    Serial.println(F("Failed! (maybe not paired?)"));
  } else {
    Serial.println(F("OK"));
  }

  // Turn off LED
  digitalWrite(LED_BUILTIN, false);
}
