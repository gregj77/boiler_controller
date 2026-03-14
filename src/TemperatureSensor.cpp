#include "TemperatureSensor.h"
#include <Arduino.h>

TemperatureSensor::TemperatureSensor(uint8_t srcPin, TemperatureReadingCallback callback) noexcept
: _srcPin(srcPin)
, _onTempChanged(std::move(callback)) {
}

TemperatureSensor::TemperatureSensor(TemperatureSensor&& other) noexcept 
: _srcPin(other._srcPin)
, _onTempChanged(std::move(other._onTempChanged)) { 

    this->_currentTemperature = other._currentTemperature;
    this->_lastReportedTemperature = other._lastReportedTemperature;
    this->_resistance = other._resistance;
    this->_firstReading = other._firstReading;
}

void TemperatureSensor::Sensors::initializeTaskLoop(std::initializer_list<TemperatureSensor> sensors) {
    if (_isInitialized) {
        return;
    }

    _isInitialized = true;

    _sensors.reserve(sensors.size());
    for (auto& sensor : sensors) {
        _sensors.emplace_back(std::move(const_cast<TemperatureSensor&>(sensor)));
    }

    analogReadResolution(12);

    xTaskCreatePinnedToCore([](void* ptr) {
        static_cast<TemperatureSensor::Sensors*>(ptr)->onTaskLoopStarted();
    }, "TempReading", 5120, this, 1, NULL, 1);
}

void TemperatureSensor::Sensors::onTaskLoopStarted() {

    TickType_t delay =  pdMS_TO_TICKS(READ_INTERVAL_MILLIS);

    while (true) {

        for (auto& sensor : _sensors) {
            sensor.checkReading();
        }
  
        vTaskDelay(delay);
    }
}


void TemperatureSensor::checkReading() {
    float mv = analogReadMilliVolts(_srcPin);
    
    if (mv <= 0 || mv >= MAX_REF_VOLTAGE) return;

    _resistance = R_SERIES * (mv / (MAX_REF_VOLTAGE - mv));

    float steinhart = _resistance / R_NOMINAL;
    steinhart = log(steinhart);
    steinhart /= B_COEFF;
    steinhart += 1.0 / (T_NOMINAL + T_KELVIN);
    steinhart = 1.0 / steinhart;
    
    float currentReading = (steinhart - 273.15);
    bool shouldNotify = false;

    if (_firstReading) {
        _currentTemperature = currentReading;
        _lastReportedTemperature = currentReading;
        _firstReading = false;
        shouldNotify = true;
    } else {
        _currentTemperature = _currentTemperature + ALPHA * (currentReading - _currentTemperature);

        if (abs(_currentTemperature - _lastReportedTemperature) >= CHANGE_DELTA) {
            shouldNotify = true;
            _lastReportedTemperature = _currentTemperature;
        }
    }

    if (shouldNotify && _onTempChanged) {
        _onTempChanged(_currentTemperature);
    }
}