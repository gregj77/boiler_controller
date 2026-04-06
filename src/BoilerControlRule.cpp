#include <Arduino.h>
#include <ArduinoLog.h>
#include <algorithm>
#include "BoilerControlRule.h"
#include "BoilerController.h"
#include "ICommandDispatcher.h"
#include "RuleSetup.h"

BoilerControlRule::BoilerControlRule(BoilerController& boilerController) noexcept
: _boilerController(boilerController) { 
    _activeRequests.reserve(8);
}

void BoilerControlRule::onInit(ICommandDispatcher& dispatcher) {
    _boilerController.setOutputTemperature(IDLE_TEMP);    
    dispatcher.registerTimer(*this, 1000, true);

    _targetTemperature = IDLE_TEMP;
    _currentOutputTemperature = IDLE_TEMP;
    _lastMeasuredTemp = 0.0f;
    _tempAtStartOfHeating = 0.0f;
    _lastSensorUpdate = 0;
    _heatingStartTime = 0;

    Log.noticeln("[BOILER_CTRL_RULE] Initialized, boiler set to idle temperature of %F °C", IDLE_TEMP);
}

void BoilerControlRule::processCommand(const Command& cmd, ICommandDispatcher& dispatcher) {
    
    if (cmd.id == CMD_BOTTOM_SENSOR_ERROR || cmd.id == CMD_TOP_SENSOR_ERROR) {
        Log.warningln("[BOILER_CTRL_RULE] Received sensor error notification, stopping boiler control rule to prevent damage");
        dispatcher.disableRule(*this);
        return;
    }

    if (cmd.id == CMD_NOTIFY_TEMP_TOP) {
        _lastMeasuredTemp = cmd.data / 100.0f;
        _lastSensorUpdate = millis();
        return;
    }

    if (cmd.id >= CMD_BOILER_25_DEGREES && cmd.id <= CMD_BOILER_OFF) {
        uint16_t ruleId = static_cast<uint16_t>(cmd.data);
        auto it = std::find(_activeRequests.begin(), _activeRequests.end(), ruleId);

        if (cmd.id == CMD_BOILER_OFF) {
            if (it != _activeRequests.end()) {
                _activeRequests.erase(it);
                Log.infoln("[BOILER_CTRL_RULE] Received boiler off command from rule %d, removing request", ruleId);
            }
        } else {
            uint8_t requestedTemp = static_cast<uint8_t>(cmd.id);
            if (it != _activeRequests.end()) {
                it->temperature = requestedTemp;
            } else {
                _activeRequests.push_back({ruleId, requestedTemp});
            }
            Log.noticeln("[BOILER_CTRL_RULE] Received boiler temperature request of %d °C from rule %d", requestedTemp, ruleId);
        }
        calculateTarget();
    }    
}

void BoilerControlRule::onTimer(ICommandDispatcher& dispatcher) {
    uint32_t now = millis();

    if (std::abs(_currentOutputTemperature - _targetTemperature) > 0.05f) {
        if (_currentOutputTemperature < _targetTemperature) {
            _currentOutputTemperature = std::min(_targetTemperature, _currentOutputTemperature + RAMP_STEP);
        } else {
            _currentOutputTemperature = std::max(_targetTemperature, _currentOutputTemperature - RAMP_STEP);
        }
        _boilerController.setOutputTemperature(_currentOutputTemperature);
    }

    if (_targetTemperature < IDLE_TEMP) { 
        // no sensor data
        if (now - _lastSensorUpdate > SENSOR_TIMEOUT_MS) {
            stop("Sensor communication timeout!");
            return;
        }

        // no heating progress
        if (now - _heatingStartTime > EFFICIENCY_TIMEOUT_MS) {
            if (_lastMeasuredTemp - _tempAtStartOfHeating < 1.5f) {
                stop("Boiler efficiency fault - temperature not rising!");
            }
        }
    }    
}

void BoilerControlRule::onStop(ICommandDispatcher& dispatcher) {
    stop("Rule stopped");
}

void BoilerControlRule::calculateTarget() {
    float oldTarget = _targetTemperature;

    if (_activeRequests.empty()) {
        _targetTemperature = IDLE_TEMP;
    } else {
        // Znajdź najniższe ID (czyli najwyższą temperaturę)
        float minTemperature = IDLE_TEMP;
        for (const auto& req : _activeRequests) {
            if (req.temperature < minTemperature) {
                minTemperature = req.temperature;
            }
        }
        _targetTemperature = minTemperature;
    }

    if (oldTarget >= IDLE_TEMP - 0.1f && _targetTemperature < IDLE_TEMP) {
        _heatingStartTime = millis();
        _tempAtStartOfHeating = _lastMeasuredTemp;
        Log.noticeln("[BOILER_CTRL_RULE] Heating cycle started. Target: %F °C", _targetTemperature);
    }    
}

void BoilerControlRule::stop(const char* reason) {
    Log.noticeln("[BOILER_CTRL_RULE] boiler control rule stop: %s", reason);
    _activeRequests.clear();
    _targetTemperature = IDLE_TEMP;
    _currentOutputTemperature = IDLE_TEMP;
    _lastMeasuredTemp = 0.0f;
    _tempAtStartOfHeating = 0.0f;
    _boilerController.setOutputTemperature(IDLE_TEMP);
}