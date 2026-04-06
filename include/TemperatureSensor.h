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
                explicit Sensors(std::initializer_list<TemperatureSensor> sensors) noexcept;
                Sensors(TemperatureSensor&& other) = delete;
                Sensors(const Sensors&) = delete;
                Sensors& operator=(Sensors&&) = delete;
                Sensors& operator=(const Sensors&) = delete;

                void setup();
                void requireSensorUpdate();

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
        const char* _name;
        float _currentTemperature = 0;
        float _lastReportedTemperature = 0;
        float _resistance = 0;
        bool _firstReading = true;
        TemperatureReadingCallback _onTempChanged;

        explicit TemperatureSensor(uint8_t srcPin, const char* name = "", TemperatureReadingCallback cb = nullptr) noexcept;

    public:

        static TemperatureSensor createTopTankSensor(TemperatureReadingCallback sensorCallback) noexcept;
        static TemperatureSensor createBottomTankSensor(TemperatureReadingCallback sensorCallback) noexcept;

        TemperatureSensor(TemperatureSensor&& other) noexcept;
        TemperatureSensor() = delete;
        TemperatureSensor(const TemperatureSensor&) = delete;
        TemperatureSensor& operator=(TemperatureSensor&&) = delete;
        TemperatureSensor& operator=(const TemperatureSensor&) = delete;

        inline float getTemp() const { return _currentTemperature; }
        inline float getResistance() const { return _resistance; }
        inline const char* getName() const { return _name; }

    private:
        void checkReading(bool notifyOutput);
};