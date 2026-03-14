#pragma once
#include <vector>
#include <functional>
#include <Arduino.h>

class TemperatureSensor final {
    public:
        using TemperatureReadingCallback = std::function<void(float)>;

        class Sensors final {
            private:
                std::vector<TemperatureSensor> _sensors;
                bool _isInitialized;

            public:
                explicit Sensors() noexcept {};
                Sensors(TemperatureSensor&& other) noexcept;
                Sensors(const Sensors&) = delete;
                Sensors& operator=(Sensors&&) = delete;
                Sensors& operator=(const Sensors&) = delete;

                void initializeTaskLoop(std::initializer_list<TemperatureSensor> sensors);

            private:
                void onTaskLoopStarted();
        };

    private:
        static constexpr float B_COEFF = 3950; // beta coefficient
        static constexpr float R_NOMINAL = 10000; // nominal termistor resistance
        static constexpr float T_NOMINAL = 25; // nominal working temperature 
        static constexpr float R_SERIES = 10000; // serial resisotor's value
        static constexpr float MAX_REF_VOLTAGE = 3300; 
        static constexpr float T_KELVIN = 273.15;
        static constexpr float ALPHA = 0.2f;
        static constexpr int READ_INTERVAL_MILLIS = 1000;
        static constexpr float CHANGE_DELTA = 0.5f;

        const uint8_t _srcPin;
        float _currentTemperature = 0;
        float _lastReportedTemperature = 0;
        float _resistance = 0;
        bool _firstReading = true;
        TemperatureReadingCallback _onTempChanged;

    public:

        explicit TemperatureSensor(uint8_t srcPin, TemperatureReadingCallback cb = nullptr) noexcept;
        TemperatureSensor(TemperatureSensor&& other) noexcept;
        TemperatureSensor() = delete;
        TemperatureSensor(const TemperatureSensor&) = delete;
        TemperatureSensor& operator=(TemperatureSensor&&) = delete;
        TemperatureSensor& operator=(const TemperatureSensor&) = delete;

        inline float getTemp() const { return _currentTemperature; }
        inline float getResistance() const { return _resistance; }

    private:
        void checkReading();
};