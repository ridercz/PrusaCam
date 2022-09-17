/* Altairis PrusaCam USB version 1.0.0 (2019-03-02)
   Copyright (c) Michal A. Valasek - Altairis, 2019
   Licensed under terms of the MIT License.
   www.rider.cz | www.altairis.cz | github.com/ridercz/PrusaCam
*/

#include <HID-Project.h>

#define TRIGGER_PIN     5       // Digital pin to use as trigger
#define TRIGGER_ON_LH   true    // Trigger on switching from LOW to HIGH
#define TRIGGER_ON_HL   false   // Trigger on switching from HIGH to LOW
#define TRIGGER_DELAY   500     // Delay after pressing trigger (kind of debounce)

bool lastTriggerState = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(TRIGGER_PIN, INPUT);
  Consumer.begin();
  digitalWrite(LED_BUILTIN, true);
}

void loop() {
  bool currentTriggerState = digitalRead(TRIGGER_PIN);
  if (lastTriggerState != currentTriggerState) {
    if ((TRIGGER_ON_LH && currentTriggerState) || (TRIGGER_ON_HL && !currentTriggerState)) {
      digitalWrite(LED_BUILTIN, false);
      Consumer.write(MEDIA_VOL_UP);
      delay(TRIGGER_DELAY);
      digitalWrite(LED_BUILTIN, true);
    }
    lastTriggerState = currentTriggerState;
  }
}
