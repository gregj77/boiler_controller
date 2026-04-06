#include "Application.h"
#include "MinTemperatureRule.h"
#include "HighProductionYieldRule.h"
#include "LegionellaRule.h"
#include "BoilerControlRule.h"
#include "HeatPumpControlRule.h"
#include "HotWaterBoostRule.h"

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

    _ruleEngine.registerRule(std::unique_ptr<MinTemperatureRule>(new MinTemperatureRule()), true);            
    _ruleEngine.registerRule(std::unique_ptr<HighProductionYieldRule>(new HighProductionYieldRule()), true);
    _ruleEngine.registerRule(std::unique_ptr<LegionellaRule>(new LegionellaRule()), false);
    _ruleEngine.registerRule(std::unique_ptr<HotWaterBoostRule>(new HotWaterBoostRule()), false);
    _ruleEngine.registerRule(std::unique_ptr<BoilerControlRule>(new BoilerControlRule(_boilerController)), true);
    _ruleEngine.registerRule(std::unique_ptr<HeatPumpControlRule>(new HeatPumpControlRule(_boilerController)), true);


    Log.noticeln("[APP] Application instance created");
}

void Application::setup() {
        Log.noticeln("[APP] Starting sensor setup....");
        _commDriver.initializeTaskLoop();
        _sensors.setup();
        _ruleEngine.init();
        _boilerController.toggleHeatPump(false);
        _boilerController.setOutputTemperature(90.0f);
        Log.noticeln("[APP] Setup complete");
}    


void Application::handleBoilerStatusChange(float outputTemp, bool relayOn) {
    _commDriver.onBoilerStatusChanged(outputTemp, relayOn);
}

void Application::handleTopTemp(float measuredTemperature) {
    if (isnan(measuredTemperature)) {
        Log.warningln("[APP] Received NaN temperature reading for top sensor");
        _topDisplay.onMeasureError();
        _commDriver.onNewTankTopTemperature(0, TempReading::Error);
        _ruleEngine.dispatchCommand({CMD_TOP_SENSOR_ERROR, 0});
        return;
    } else {
        auto reading = _topDisplay.onNewTemperatureReading(measuredTemperature);
        _commDriver.onNewTankTopTemperature(measuredTemperature, reading);
        _ruleEngine.dispatchCommand({CMD_NOTIFY_TEMP_TOP, static_cast<int32_t>(measuredTemperature * 100)});
    }
}

void Application::handleBottomTemp(float measuredTemperature) {
    if (isnan(measuredTemperature)) {
        Log.warningln("[APP] Received NaN temperature reading for bottom sensor");
        _bottomDisplay.onMeasureError();
        _commDriver.onNewTankBottomTemperature(0, TempReading::Error);
        _ruleEngine.dispatchCommand({CMD_BOTTOM_SENSOR_ERROR, 0});
        return;
    } else {
        auto reading = _bottomDisplay.onNewTemperatureReading(measuredTemperature);
        _commDriver.onNewTankBottomTemperature(measuredTemperature, reading);
        _ruleEngine.dispatchCommand({CMD_NOTIFY_TEMP_BOTTOM, static_cast<int32_t>(measuredTemperature * 100)});
    }
}

void Application::processLoop() {
    _ruleEngine.processCommands();
}