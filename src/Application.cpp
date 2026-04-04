#include "Application.h"
#include "MinTemperatureRule.h"
#include "BoilerControlRule.h"
#include "HighProductionYieldRule.h"

#include <Arduino.h>
#include <ArduinoLog.h>
#include <memory>

#define SLAVE_ID 77

Application::Application() noexcept: 
        _commDriver(SLAVE_ID, _ruleEngine),
        _topDisplay(TemperatureDisplay::createTopTemperatureDisplay()),
        _bottomDisplay(TemperatureDisplay::createBottomTemperatureDisplay()),
        _boilerController([this](float temperature, bool relay) { 
            handleBoilerStatusChange(temperature, relay);  
        }),
        _sensors({
            TemperatureSensor::createTopTankSensor([this](float temp) { 
                handleTopTemp(temp); 
            }), 
            TemperatureSensor::createBottomTankSensor([this](float temp) { 
                handleBottomTemp(temp); 
            })
        }) {

    _ruleEngine.registerRule(std::unique_ptr<MinTemperatureRule>(new MinTemperatureRule()), false);            
    _ruleEngine.registerRule(std::unique_ptr<HighProductionYieldRule>(new HighProductionYieldRule()), true);
    _ruleEngine.registerRule(std::unique_ptr<BoilerControlRule>(new BoilerControlRule(_boilerController)), true);


    Log.noticeln("[APP] Application instance created");
}

void Application::setup() {
        Log.noticeln("[APP] Starting sensor setup....");
        _commDriver.initializeTaskLoop();
        _sensors.setup();
        _ruleEngine.init();
        Log.noticeln("[APP] Setup complete");
}    


void Application::handleBoilerStatusChange(float outputTemp, bool relayOn) {
    _commDriver.onBoilerStatusChanged(outputTemp, relayOn);
}

void Application::handleTopTemp(float measuredTemperature) {
    auto reading = _topDisplay.onNewTemperatureReading(measuredTemperature);
    _commDriver.onNewTankTopTemperature(measuredTemperature, reading);
    _ruleEngine.dispatchCommand({CMD_NOTIFY_TEMP_TOP, static_cast<int32_t>(measuredTemperature * 100)});
}

void Application::handleBottomTemp(float measuredTemperature) {
    auto reading = _bottomDisplay.onNewTemperatureReading(measuredTemperature);
    _commDriver.onNewTankBottomTemperature(measuredTemperature, reading);
    _ruleEngine.dispatchCommand({CMD_NOTIFY_TEMP_BOTTOM, static_cast<int32_t>(measuredTemperature * 100)});
}

void Application::processLoop() {
    _ruleEngine.processCommands();
}