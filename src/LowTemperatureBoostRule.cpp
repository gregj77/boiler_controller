#include "LowTemperatureBoostRule.h"
#include "RuleEngine.h"
#include <ArduinoLog.h>

LowTemperatureBoostRule::LowTemperatureBoostRule() noexcept
: _isActive(false) {
}

void LowTemperatureBoostRule::processCommand(const Command& cmd, ICommandDispatcher& dispatcher) {
    if (cmd.id == CMD_NOTIFY_TEMP_TOP) {
        _currentTemperature = cmd.data / 100.0f;

        if (_isActive) {
            if (_currentTemperature  >= EXPECTED_TEMP) {
                _isActive = false;
                dispatcher.dispatchCommand({CMD_BOILER_OFF, getId()}); 
                Log.infoln("[LOW_TEMP_BOOST_RULE] Temperature back to normal (%F °C), deactivating rule", _currentTemperature);
            }
        }
        else {
            if (_currentTemperature <= MIN_TEMP_THRESHOLD) {
                _isActive = true;
                dispatcher.dispatchCommand({CMD_BOILER_35_DEGREES, getId()}); 
                Log.infoln("[LOW_TEMP_BOOST_RULE] Temperature dropped below threshold (%F °C), activating rule", _currentTemperature);
            }
        }
    }
}   

void LowTemperatureBoostRule::onStop(ICommandDispatcher& dispatcher) {
    if (_isActive) {
        _isActive = false;
        dispatcher.dispatchCommand({CMD_BOILER_OFF, getId()}); 
        Log.noticeln("[LOW_TEMP_BOOST_RULE] Rule stopped, ensuring boiler is turned off");
    }
}