#pragma once

#include <Arduino.h>

enum TempReading {
    High = 3,
    Medium = 2,
    Low = 1
};

class TemperatureDisplay final{
    private:
        const uint8_t _coldTempLedPin;
        const uint8_t _midTempLedPin;
        const uint8_t _highTempLedPin;

        const float _lowToMedTempLimit;
        const float _medToHighTempLimit;
        const char* _name;


    public:
        explicit TemperatureDisplay(const char* name, uint8_t coldTempLedPin, uint8_t midTempLedPin, uint8_t highTempLedPin, float lowToMedTempLimit = 25, float medToHighTempLimit = 35) noexcept;

        TempReading onNewTemperatureReading(float temperature);

        TemperatureDisplay() = delete;
        TemperatureDisplay(const TemperatureDisplay&) = delete;
        TemperatureDisplay(TemperatureDisplay&&) = delete;
        TemperatureDisplay& operator=(const TemperatureDisplay&) = delete;
        TemperatureDisplay& operator=(TemperatureDisplay&&) = delete;

};