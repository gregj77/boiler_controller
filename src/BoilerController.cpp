#include "BoilerController.h"
#include <SPI.h>
#include <ArduinoLog.h>

constexpr BoilerController::NtcPoint BoilerController::NTC_TABLE[];

BoilerController::BoilerController(BoilerStatusCallback callback) noexcept : _onStatusChanged(callback) {
    pinMode(RELAY_PIN, OUTPUT_OPEN_DRAIN);
    digitalWrite(RELAY_PIN, HIGH);
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);
    // SPI.begin(SCK, MISO, MOSI, CS) - MISO set to -1 => unused! 
    SPI.begin(SCK_PIN, -1, MOSI_PIN, CS_PIN);
}

void BoilerController::setOutputTemperature(float temperature) {
    
    mapTemperature(temperature, _currentTemp, _digitalOutputValue);

    digitalWrite(CS_PIN, LOW);
    SPI.transfer(0x11);      // command: 0x11 - write data to channel 0
    uint8_t value = static_cast<uint8_t>(_digitalOutputValue);
    SPI.transfer(value);     // value of the mapped temperature 
    digitalWrite(CS_PIN, HIGH);

    Log.traceln("[BOILER_CTRL] Set output temperature: %F °C (mapped to %d)", temperature, value);

    // Notify callback if set
    if (_onStatusChanged) {
        _onStatusChanged(_currentTemp, _relayEnabled);
    }
}

void BoilerController::toggleHeatPump(bool enable) {
    _relayEnabled = enable;
    digitalWrite(RELAY_PIN, enable ? LOW : HIGH);

    Log.traceln("[BOILER_CTRL] Heat pump turned %s", enable ? "ON" : "OFF");

    // Notify callback if set
    if (_onStatusChanged) {
        _onStatusChanged(_currentTemp, _relayEnabled);
    }
}

void BoilerController::mapTemperature(float targetTemp, float& adjustedTemperature, uint8_t& digitalOutputValue) const {

    if (targetTemp <= NTC_TABLE[0].temp) {
        adjustedTemperature = NTC_TABLE[0].temp;
        digitalOutputValue = 0;
        return;
    }

    float targetResistance;
    if (targetTemp >= NTC_TABLE[NTC_TABLE_SIZE - 1].temp) {
        adjustedTemperature = NTC_TABLE[NTC_TABLE_SIZE - 1].temp;
        targetResistance = NTC_TABLE[NTC_TABLE_SIZE - 1].ohms;
    } else {
        adjustedTemperature = targetTemp;

        int i = 0;
        while (i < NTC_TABLE_SIZE - 1 && targetTemp > NTC_TABLE[i + 1].temp) {
            i++;
        }

        const NtcPoint& p0 = NTC_TABLE[i];
        const NtcPoint& p1 = NTC_TABLE[i + 1];

        targetResistance = p0.ohms + (targetTemp - p0.temp) * (p1.ohms - p0.ohms) / (p1.temp - p0.temp);
    }

    float stepsFloat = 255.0f * (1.0f - (targetResistance / MCP_MAX_OHMS));

    int steps = (int)(stepsFloat + 0.5f); 
    if (steps < 0) {
        digitalOutputValue = 0;
    } else if (steps > 255) {
        digitalOutputValue = 255;
    } else {
        digitalOutputValue = static_cast<uint8_t>(steps);
    }
}
