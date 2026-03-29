#pragma once
#include "RuleEngine.h"
#include "ModbusDriver.h"
#include "BoilerController.h"
#include "TemperatureDisplay.h"
#include "TemperatureSensor.h"
#include <Arduino.h>
#include <memory>

class Application final {
private:
    RuleEngine _ruleEngine;
    ModbusDriver _commDriver;

    BoilerController _boilerController;    
    TemperatureDisplay _topDisplay;
    TemperatureDisplay _bottomDisplay;
    
    TemperatureSensor::Sensors _sensors;

public:
    explicit Application() noexcept;

    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    void setup();
    void processLoop();

private:
    void handleTopTemp(float measuredTemperature);

    void handleBottomTemp(float measuredTemperature);
    
    void handleBoilerStatusChange(float outputTemp, bool relayOn);
};