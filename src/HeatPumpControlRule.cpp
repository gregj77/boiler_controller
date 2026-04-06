#include <Arduino.h>
#include <ArduinoLog.h>
#include <algorithm>
#include "HeatPumpControlRule.h"
#include "BoilerController.h"
#include "RuleEngine.h"


HeatPumpControlRule::HeatPumpControlRule(BoilerController& boilerController) noexcept
: _boilerController(boilerController) {
    _activeRequests.reserve(4);
}   

void HeatPumpControlRule::onStop(ICommandDispatcher& dispatcher) {
    Log.noticeln("[BOILER_RULE] Stopping rule, clearing all heat pump requests and turning off heat pump");
    _activeRequests.clear();
    _boilerController.toggleHeatPump(false);
}

void HeatPumpControlRule::processCommand(const Command& cmd, ICommandDispatcher& dispatcher) {
    
    switch (cmd.id)
    {
    case CMD_RELAY_CTRL_ON:
        addHeatPumpRequest(cmd.data);
        break;
    case CMD_RELAY_CTRL_OFF:
        removeHeatPumpRequest(cmd.data);
        break;
    case CMD_TOP_SENSOR_ERROR:
    case CMD_BOTTOM_SENSOR_ERROR:
        Log.warningln("[BOILER_RULE] Received sensor error notification, stopping boiler control rule to prevent damage");
        dispatcher.disableRule(*this);
        break;
    default:
        break;
    }
}

void HeatPumpControlRule::addHeatPumpRequest(uint16_t ruleId) {
    auto it = std::find(_activeRequests.begin(), _activeRequests.end(), ruleId);
    
    if (it != _activeRequests.end()) {
        Log.noticeln("[BOILER_RULE] Updated heat pump request from rule ID: %d", ruleId);
        return;
    } else {
        Log.noticeln("[BOILER_RULE] Adding new heat pump request from rule ID: %d", ruleId);
        _activeRequests.push_back({ruleId});
    }

    Log.infoln("[BOILER_RULE] Heat pump activated due to new request");
    _boilerController.toggleHeatPump(true);
}

void HeatPumpControlRule::removeHeatPumpRequest(uint16_t ruleId) {    
    bool wasActive = !_activeRequests.empty();
    _activeRequests.erase(std::remove(_activeRequests.begin(), _activeRequests.end(), ruleId), _activeRequests.end());
    if (wasActive && _activeRequests.empty()) {
        Log.infoln("[BOILER_RULE] Heat pump deactivated as all requests have expired");
        _boilerController.toggleHeatPump(false);
    }
}