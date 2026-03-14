#include "BoilerController.h"
#include <SPI.h>
#include <ArduinoLog.h>


BoilerController::BoilerController() noexcept {
    pinMode(RELAY_PIN, OUTPUT_OPEN_DRAIN);
    digitalWrite(RELAY_PIN, HIGH);
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);
    // SPI.begin(SCK, MISO, MOSI, CS) - MISO set to -1 => unused! 
    SPI.begin(SCK_PIN, -1, MOSI_PIN, CS_PIN);
}

void BoilerController::setOutputTemperature(float temperature) {
    _currentTemp = temperature;
    _outputTemp = mapTemperature(temperature);

    digitalWrite(CS_PIN, LOW);
    SPI.transfer(0x11);      // command: 0x11 - write data to channel 0
    uint8_t value = static_cast<uint8_t>(_outputTemp);
    SPI.transfer(value);     // value of the mapped temperature - 
    digitalWrite(CS_PIN, HIGH);
    Log.notice("Potencjometr: %d\n", value);
}

void BoilerController::toggleHeatPump(bool enable) {
    digitalWrite(RELAY_PIN, enable ? LOW : HIGH);
}

float BoilerController::mapTemperature(float temperature) const {
    if (temperature <= _inMin) return _outMin;
    if (temperature >= _inMax) return _outMax;

    return (temperature - _inMin) * (_outMax - _outMin) / (_inMax - _inMin) + _outMin;
}
