#pragma once
#include <Arduino.h>

class BoilerController final {
    private:
        static constexpr uint8_t MOSI_PIN = 40;
        static constexpr uint8_t SCK_PIN = 41;
        static constexpr uint8_t CS_PIN = 42;
        static constexpr uint8_t RELAY_PIN = 38;

        float _inMin = 0.0;
        float _inMax = 80.0;

        float _outMin = 0.0;
        float _outMax = 255.0;

        float _currentTemp;
        float _outputTemp;        
    public:

        explicit BoilerController() noexcept;

        void setOutputTemperature(float temperature);

        void toggleHeatPump(bool enable);

        BoilerController(const BoilerController&) = delete;
        BoilerController(BoilerController&&) = delete;
        BoilerController& operator=(const BoilerController&) = delete;
        BoilerController& operator=(BoilerController&&) = delete;

    private:
        float mapTemperature(float temperature) const;

};