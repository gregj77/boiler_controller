#include <Arduino.h>
#include "esp_heap_caps.h"
#include <ModbusSerial.h>
#include <SPI.h>
#include "TemperatureSensor.h"
#include "TemperatureDisplay.h"
#include "ModbusDriver.h"
#include "BoilerController.h"
#include <vector>
#include "driver/uart.h"
#include <ArduinoLog.h>

#define NTC_TANK_BOTTOM_PIN 4
#define NTC_TANK_UP_PIN 5

#define TANK_BOTTOM_LOW_TEMP 12
#define TANK_BOTTOM_MED_TEMP 13
#define TANK_BOTTOM_HIGH_TEMP 14

#define TANK_TOP_LOW_TEMP 37 
#define TANK_TOP_MED_TEMP 36
#define TANK_TOP_HIGH_TEMP 35


// 3. SPI MCP41010 (Prawa strona płytki: 40, 41, 42)
// PAMIĘTAJ: Zasil MCP (pin 8) z 5V, aby obsłużyć 5V z kotła!
#define MOSI_PIN 40
#define SCK_PIN  41
#define CS_PIN   42


// Definicje pinów
#define RX2_PIN 16
#define TX2_PIN 17
#define DIR_PIN 18
#define SLAVE_ID 77
#define UART_PORT UART_NUM_2


const int Lamp1Coil = 0;
#define COIL_RED    1
#define COIL_YELLOW  2
#define COIL_BLUE   3
#define COIL_RED_2    4
#define COIL_YELLOW_2  5
#define COIL_BLUE_2   6
#define HREG_POT_VAL 0


TemperatureSensor::Sensors temperatureSensors;
BoilerController boilerController;
TemperatureDisplay topTankDisplay("top", TANK_TOP_LOW_TEMP, TANK_TOP_MED_TEMP, TANK_TOP_HIGH_TEMP, 33.0f, 45.0f);
TemperatureDisplay bottomTankDisplay("bottom", TANK_BOTTOM_LOW_TEMP, TANK_BOTTOM_MED_TEMP, TANK_BOTTOM_HIGH_TEMP, 20.0f, 30.0f);
ModbusDriver commDriver(SLAVE_ID);

void setup() {

  commDriver.initializeTaskLoop();

  std::function<void(float)> onTopTempChangedHandler = [](float temp) {
    TempReading reading = topTankDisplay.onNewTemperatureReading(temp); 
    commDriver.onNewTankTopTemperature(temp, reading);
  };

  std::function<void(float)> onBottomTempChangedHandler = [](float temp) {
    TempReading reading = bottomTankDisplay.onNewTemperatureReading(temp); 
    commDriver.onNewTankBottomTemperature(temp, reading);
  };

  TemperatureSensor topSensor(NTC_TANK_UP_PIN, std::move(onTopTempChangedHandler));
  TemperatureSensor botomSensor(NTC_TANK_BOTTOM_PIN, std::move(onBottomTempChangedHandler));

  temperatureSensors.initializeTaskLoop({ std::move(topSensor), std::move(botomSensor)});

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(115200);

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  //pinMode(RX2_PIN, INPUT);
  //pinMode(TX2_PIN, OUTPUT);

  /*
  size_t internal_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
  size_t internal_largest = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);  

  // PSRAM (if initialized)
  size_t psram_size =  ESP.getPsramSize();
  size_t psram_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
  size_t psram_largest = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);
  
  
  Serial.println("=== MEMORY INFO ===");
  Serial.printf("Internal SRAM free: %u bytes\n", internal_free);
  Serial.printf("Internal SRAM largest free block: %u bytes\n", internal_largest);

  if (psram_size > 0) {
    Serial.printf("PSRAM detected: %u bytes total\n", psram_size);
    Serial.printf("PSRAM free: %u bytes\n", psram_free);
    Serial.printf("PSRAM largest free block: %u bytes\n", psram_largest);
  } else {
    Serial.println("No PSRAM detected");
  }
    */


  
  //pinMode(DIR_PIN, OUTPUT);
  //Serial.println("Starting RS485 test");

//  pinMode(CS_PIN, OUTPUT);
//  digitalWrite(CS_PIN, HIGH);
//   SPI.begin(SCK, MISO, MOSI, CS) - MISO ustawiamy na -1 (nieużywane)
  //SPI.begin(SCK_PIN, -1, MOSI_PIN, CS_PIN);
//  pinMode(13, OUTPUT);


  //xTaskCreatePinnedToCore(TemperatureSensor::taskLoop, "TempReading", /*2048*/5120, &temperatureSensors, 1, NULL, 1);
  Log.info("Setup complete!");
}


void loop3() {
  // TRANSMIT MODE
  digitalWrite(DIR_PIN, HIGH);
  delay(10);
  Serial2.println("hello world!");  // 01010101 pattern
  Serial2.flush();
  delay(10);

  // RECEIVE MODE
  digitalWrite(DIR_PIN, LOW);
  Serial.println(".");
  delay(1000);
}

const float TEMP_MIN = 20.0;
const float TEMP_MAX = 33.0;

void loopsdfsdf() {

    // Wymuszamy tryb OUTPUT bezpośrednio tutaj, by wykluczyć błąd w klasie
    //pinMode(38, OUTPUT); 
  //  vTaskDelay(pdMS_TO_TICKS(10000));
    BoilerController boilerController;
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(5000));
    boilerController.toggleHeatPump(true);
    vTaskDelay(pdMS_TO_TICKS(10000));
    digitalWrite(LED_BUILTIN, LOW);
    boilerController.toggleHeatPump(false);
    vTaskDelay(pdMS_TO_TICKS(10000));
    vTaskDelete(NULL);

    //BoilerController boilerController;
  
    while(true) {
      Serial.printf("tick!");
      for (int i = 0; i < 80; ++i) {
        digitalWrite(LED_BUILTIN, i % 2 == 0);
        boilerController.setOutputTemperature(i);
        vTaskDelay(pdMS_TO_TICKS(1000));
      }
      for (int i = 0; i < 80; ++i) {
        digitalWrite(LED_BUILTIN, i % 2 == 0);
        boilerController.setOutputTemperature(80 - i);
        vTaskDelay(pdMS_TO_TICKS(1000));
      }
        
    }
}


void loop() {
  for (int i = 0; i < 10; i++) {
    vTaskDelay(pdMS_TO_TICKS(1500));
    Serial.println("pin 38 - HIGH");
    boilerController.toggleHeatPump(true);
    vTaskDelay(pdMS_TO_TICKS(1500));
    Serial.println("pin 38 - LOW");
    boilerController.toggleHeatPump(false);
  }
  vTaskDelete(NULL);
}
