#pragma once

#include <Arduino.h>
#include <memory>

enum TempReading {
    High = 3,
    Medium = 2,
    Low = 1
};

class TemperatureDisplay final {
    private:
        const uint8_t _coldTempLedPin;
        const uint8_t _midTempLedPin;
        const uint8_t _highTempLedPin;

        const float _lowToMedTempLimit;
        const float _medToHighTempLimit;
        const char* _name;
        explicit TemperatureDisplay(const char* name, uint8_t coldTempLedPin, uint8_t midTempLedPin, uint8_t highTempLedPin, float lowToMedTempLimit = 25, float medToHighTempLimit = 35) noexcept;

        struct DisplayData : public Printable {
            float temperature; 
            TempReading status;
            DisplayData(float temp, TempReading reading);
            size_t printTo(Print &p) const;
        };

    public:
        static TemperatureDisplay createTopTemperatureDisplay() noexcept;
        static TemperatureDisplay createBottomTemperatureDisplay() noexcept;

        TempReading onNewTemperatureReading(float temperature);

        TemperatureDisplay() = delete;
        TemperatureDisplay(const TemperatureDisplay&) = delete;
        TemperatureDisplay(TemperatureDisplay&&) = default;
        TemperatureDisplay& operator=(const TemperatureDisplay&) = delete;
        TemperatureDisplay& operator=(TemperatureDisplay&&) = delete;

};