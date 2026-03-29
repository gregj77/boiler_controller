#include <Arduino.h>
#include <ArduinoLog.h>
#include "TemperatureSensor.h"

#define NTC_TANK_BOTTOM_PIN 4
#define NTC_TANK_UP_PIN 5


TemperatureSensor TemperatureSensor::createTopTankSensor(TemperatureReadingCallback sensorCallback) noexcept {
    return TemperatureSensor(NTC_TANK_UP_PIN, "top", std::move(sensorCallback));
}

TemperatureSensor TemperatureSensor::createBottomTankSensor(TemperatureReadingCallback sensorCallback) noexcept {
    return TemperatureSensor(NTC_TANK_BOTTOM_PIN, "bottom", std::move(sensorCallback));
}

TemperatureSensor::Sensors::Sensors(std::initializer_list<TemperatureSensor> sensors) noexcept {
    _sensors.reserve(sensors.size());
    for (auto& sensor : sensors) {
        _sensors.emplace_back(std::move(const_cast<TemperatureSensor&>(sensor)));
    }
}

TemperatureSensor::TemperatureSensor(uint8_t srcPin, const String& name, TemperatureReadingCallback callback) noexcept
: _srcPin(srcPin)
, _name(name)
, _onTempChanged(std::move(callback)) {
}

TemperatureSensor::TemperatureSensor(TemperatureSensor&& other) noexcept 
: _srcPin(other._srcPin)
, _name(std::move(other._name))
, _onTempChanged(std::move(other._onTempChanged)) { 

    this->_currentTemperature = other._currentTemperature;
    this->_lastReportedTemperature = other._lastReportedTemperature;
    this->_resistance = other._resistance;
    this->_firstReading = other._firstReading;
}

void TemperatureSensor::Sensors::setup() {
    if (_isInitialized) {
        return;
    }

    _isInitialized = true;

    analogReadResolution(12);

    xTaskCreatePinnedToCore([](void* ptr) {
        static_cast<TemperatureSensor::Sensors*>(ptr)->onTaskLoopStarted();
    }, "TempReading", 5120, this, 1, NULL, 1);
}

void TemperatureSensor::Sensors::onTaskLoopStarted() {

    TickType_t delay =  pdMS_TO_TICKS(READ_INTERVAL_MILLIS);
    vTaskDelay(delay * 5);
    Log.noticeln("[TEMP] Task loop started with %d sensors", _sensors.size());
    int i = 0; 

    while (true) {

        for (auto& sensor : _sensors) {
            sensor.checkReading(i == 0);
        }
        if (i >= 10) { i = 0; } else { ++i; };
  
        vTaskDelay(delay);
    }
}


void TemperatureSensor::checkReading(bool notifyOutput) {
    float mv = analogReadMilliVolts(_srcPin);
    
    if (mv <= 0 || mv >= MAX_REF_VOLTAGE) {
        Log.warningln("[TEMP] sensor=%s invalid reading: %F mV", _name.c_str(), mv);
        return;        
    }

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

    if (shouldNotify) {
        Log.noticeln("[TEMP] update reading - sensor=%s current=%F °C resistance=%F Ω", _name.c_str(), _currentTemperature, _resistance);
    } else if (notifyOutput){
        Log.verboseln("[TEMP] sensor=%s current=%F °C resistance=%F Ω", _name.c_str(), _currentTemperature, _resistance);
    }

    if (shouldNotify && _onTempChanged) {
        _onTempChanged(_currentTemperature);
    }
}