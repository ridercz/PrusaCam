/* Altairis PrusaCam MK4 ESP32 BT version 4.0.0 (2024-09-08)
   Copyright (c) Michal A. Valasek - Altairis, 2021-2024
   Licensed under terms of the MIT License.
   www.rider.cz | www.altairis.cz | github.com/ridercz/PrusaCam
*/

#include <Arduino.h>
#include <BleKeyboard.h>

#define LED_PIN 8                       // GPIO pin for LED
#define LED_ON LOW                      // LED is on when LOW
#define LED_OFF HIGH                    // LED is off when HIGH
#define TRIGGER_PIN 5                   // GPIO pin used to trigger
#define TRIGGER_ON_LH true              // trigger on rising edge
#define TRIGGER_ON_HL false             // trigger on falling edge
#define TRIGGER_WAIT 500                // wait after trigger (ms)
#define INITIAL_DELAY 1000              // ignore pin changes for n miliseconds after start
#define SHUTTER_KEY KEY_MEDIA_VOLUME_UP // KEY_MEDIA_VOLUME_UP for Android, KEY_RETURN for Windows Phone
#define PRODUCT_NAME "PrusaCam MK4"     // BLE device name
#define PRODUCT_VERSION "4.0.0"         // BLE device version
#define PRODUCT_MANUFACTURER "Altairis" // BLE device manufacturer

BleKeyboard bleKeyboard(PRODUCT_NAME, PRODUCT_MANUFACTURER);
bool lastTriggerState;
bool delay_passed = false;

void pressShutter()
{
  // Turn on LED
  digitalWrite(LED_PIN, LED_ON);

  // Pres shutter
  if (bleKeyboard.isConnected())
  {
    Serial.println("Pressing shutter...");
    bleKeyboard.write(SHUTTER_KEY);
  }
  else
  {
    Serial.println("Would press shutter but keyboard is not connected.");
  }

  // Wait for a while and then turn off LED
  delay(TRIGGER_WAIT);
  digitalWrite(LED_PIN, LED_OFF);
}

void setup()
{
  // Show header
  Serial.begin(9600);
  Serial.println();
  Serial.println();
  Serial.println(PRODUCT_NAME " version " PRODUCT_VERSION);
  Serial.println("Copyright (c) Michal A. Valasek - Altairis, 2021-2024");
  Serial.println("www.rider.cz | www.altair.blog | github.com/ridercz/PrusaCam");
  Serial.println();

  // Display configuration
  Serial.print("Pin number: ");
  Serial.println(TRIGGER_PIN);
  if (TRIGGER_ON_LH)
    Serial.println("Will trigger on L->H.");
  if (TRIGGER_ON_HL)
    Serial.println("Will trigger on H->L.");
  if (!(TRIGGER_ON_LH || TRIGGER_ON_HL))
    Serial.println("Both TRIGGER_ON_LH and TRIGGER_ON_HL are disabled. The device will do nothing.");
  Serial.println();

  // Start BLE keyboard
  bleKeyboard.begin();

  // Configure trigger pin
  pinMode(TRIGGER_PIN, INPUT);
  lastTriggerState = digitalRead(TRIGGER_PIN);

  // Turn on LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_ON);
}

void loop()
{
  // Check if initial delay had already passed
  if (!delay_passed && millis() > INITIAL_DELAY)
  {
    delay_passed = true;
    digitalWrite(LED_PIN, LED_OFF);
  }
  if (!delay_passed)
    return;

  // Check if we have state change
  bool currentTriggerState = digitalRead(TRIGGER_PIN);
  if (lastTriggerState == currentTriggerState)
    return;

  // Press shutter on rising/falling edge
  if (currentTriggerState && TRIGGER_ON_LH)
    pressShutter();
  if (!currentTriggerState && TRIGGER_ON_HL)
    pressShutter();

  // Remember the state
  lastTriggerState = currentTriggerState;
}
