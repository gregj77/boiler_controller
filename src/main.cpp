#include <Arduino.h>
#include "esp_heap_caps.h"
#include <ArduinoLog.h>
#include "Application.h"


Application app;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(115200);
  //Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  Log.begin(LOG_LEVEL_TRACE, &Serial);

  app.setup();
  Log.info("Setup complete!");
}

void loop() {
  app.processLoop();
}
