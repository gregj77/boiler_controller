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

        struct NtcPoint {
            float temp;
            float ohms;
        };

        static constexpr float MCP_MAX_OHMS = 10000.0f;
        static constexpr int NTC_TABLE_SIZE = 8;
        static constexpr NtcPoint NTC_TABLE[NTC_TABLE_SIZE] = {
            {25.2f, 10000.0f}, {30.0f, 7947.0f}, {40.0f, 5242.0f}, {50.0f, 3548.0f},
            {60.0f, 2459.0f},  {70.0f, 1740.0f}, {80.0f, 1256.0f}, {90.0f, 923.0f}
        };

        float _currentTemp;
        uint8_t _digitalOutputValue;        
        bool _relayEnabled = false; 
        BoilerStatusCallback _onStatusChanged;

    public:
        explicit BoilerController(BoilerStatusCallback callback = nullptr) noexcept;

        void setOutputTemperature(float temperature);

        void toggleHeatPump(bool enable);

        float getOutputTemperature() const { return _currentTemp; }
        bool isHeatPumpEnabled() const { return _relayEnabled; }

        BoilerController(const BoilerController&) = delete;
        BoilerController(BoilerController&&) = delete;
        BoilerController& operator=(const BoilerController&) = delete;
        BoilerController& operator=(BoilerController&&) = delete;

    private:
        void mapTemperature(float targetTemp, float& adjustedTemperature, uint8_t& digitalOutputValue) const;
};