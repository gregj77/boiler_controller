#include "MinTemperatureRule.h"
#include "BoilerController.h"
#include "RuleEngine.h"
#include <ArduinoLog.h>

MinTemperatureRule::MinTemperatureRule() noexcept
: _isActive(false) {
}

void MinTemperatureRule::processCommand(const Command& cmd, ICommandDispatcher& dispatcher) {
    if (cmd.id == CMD_NOTIFY_TEMP_TOP) {
        _currentTemperature = cmd.data / 100.0f;

        if (_isActive) {
            if (_currentTemperature  >= EXPECTED_TEMP) {
                _isActive = false;
                dispatcher.dispatchCommand({CMD_RELAY_CTRL_OFF, getId()}); 
                Log.infoln("[MIN_TEMP_RULE] Temperature back to normal (%F °C), deactivating rule", _currentTemperature);
            }
        }
        else {
            if (_currentTemperature <= MIN_TEMP_THRESHOLD) {
                _isActive = true;
                dispatcher.dispatchCommand({CMD_RELAY_CTRL_ON, getId()}); 
                Log.infoln("[MIN_TEMP_RULE] Temperature dropped below threshold (%F °C), activating rule", _currentTemperature);
            }
        }
    }
}   

void MinTemperatureRule::stop(ICommandDispatcher& dispatcher) {
    if (_isActive) {
        _isActive = false;
        dispatcher.dispatchCommand({CMD_RELAY_CTRL_OFF, getId()}); 
        Log.noticeln("[MIN_TEMP_RULE] Rule stopped, ensuring heat pump is turned off");
    }
}