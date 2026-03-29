#pragma once
#include <Arduino.h>
#include <functional>

class BoilerController final {
    public:
        using BoilerStatusCallback = std::function<void(float outputTemp, bool relayOn)>;
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
        bool _relayEnabled = false; 
        BoilerStatusCallback _onStatusChanged;

    public:
        explicit BoilerController(BoilerStatusCallback callback = nullptr) noexcept;

        void setOutputTemperature(float temperature);

        void toggleHeatPump(bool enable);

        float getOutputTemperature() const { return _outputTemp; }
        bool isHeatPumpEnabled() const { return _relayEnabled; }

        BoilerController(const BoilerController&) = delete;
        BoilerController(BoilerController&&) = delete;
        BoilerController& operator=(const BoilerController&) = delete;
        BoilerController& operator=(BoilerController&&) = delete;

    private:
        float mapTemperature(float temperature) const;
};