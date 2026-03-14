#include "TemperatureDisplay.h"

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

    if (Serial) {
        const char* option =  
            temperature <= _lowToMedTempLimit 
            ? "cold" 
            : temperature > _lowToMedTempLimit && temperature <= _medToHighTempLimit 
                ? "middle" 
                : "high";

        Serial.printf("%s - temp %.1f - %s %d\n", _name, temperature, option, (int)result);
    }

    return result;
}