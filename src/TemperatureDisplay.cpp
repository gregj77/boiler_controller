#include "TemperatureDisplay.h"
#include <memory>
#include <ArduinoLog.h>

#define TOP_COLD_PIN 37
#define TOP_MED_PIN 36
#define TOP_HIGH_PIN 35
#define TOP_LOW_LIMIT 33.0f
#define TOP_HIGH_LIMIT 45.0f

#define BOTTOM_COLD_PIN 12
#define BOTTOM_MED_PIN 13
#define BOTTOM_HIGH_PIN 14
#define BOTTOM_LOW_LIMIT 20.0f
#define BOTTOM_HIGH_LIMIT 30.0f

TemperatureDisplay::TemperatureDisplay(const char* name, uint8_t coldTempLedPin, uint8_t midTempLedPin, uint8_t highTempLedPin, float lowToMedTempLimit, float medToHighTempLimit) noexcept
: _name(name)
, _coldTempLedPin(coldTempLedPin)
, _midTempLedPin(midTempLedPin)
, _highTempLedPin(highTempLedPin)
, _lowToMedTempLimit(lowToMedTempLimit)
, _medToHighTempLimit(medToHighTempLimit) {

  pinMode(coldTempLedPin, OUTPUT); // blue
  pinMode(midTempLedPin, OUTPUT); // yellow
  pinMode(highTempLedPin, OUTPUT); // red
};

TemperatureDisplay TemperatureDisplay::createTopTemperatureDisplay() noexcept {
    return TemperatureDisplay("top", TOP_COLD_PIN, TOP_MED_PIN, TOP_HIGH_PIN, TOP_LOW_LIMIT, TOP_HIGH_LIMIT);
}

TemperatureDisplay TemperatureDisplay::createBottomTemperatureDisplay() noexcept {
    return TemperatureDisplay("bottom", BOTTOM_COLD_PIN, BOTTOM_MED_PIN, BOTTOM_HIGH_PIN, BOTTOM_LOW_LIMIT, BOTTOM_HIGH_LIMIT);
}

TempReading TemperatureDisplay::onNewTemperatureReading(float temperature) {
    bool isCold = temperature <= _lowToMedTempLimit;
    bool isMid = temperature > _lowToMedTempLimit && temperature <= _medToHighTempLimit;
    bool isHigh = temperature > _medToHighTempLimit;
    digitalWrite(_coldTempLedPin, isCold);
    digitalWrite(_midTempLedPin, isMid);
    digitalWrite(_highTempLedPin, isHigh);


    TempReading result = isHigh 
    ? TempReading::High 
    : isCold 
        ? TempReading::Low 
        : TempReading::Medium;

    DisplayData data(temperature, result);
    Log.notice("[DISPLAY] %s: %p\n", _name, data);

    return result;
}

TemperatureDisplay::DisplayData:: DisplayData(float temp, TempReading reading): temperature(temp), status(reading) {
}

size_t TemperatureDisplay::DisplayData::printTo(Print &p) const {
    size_t bytesSent = 0;
    bytesSent += p.print("temp: ");
    bytesSent += p.print(temperature);
    bytesSent += p.print("°C, status: ");
    const char* statusStr = status == TempReading::High ? "HIGH" : status == TempReading::Medium ? "MEDIUM" : "LOW";
    bytesSent += p.print(statusStr);
    return bytesSent;
}