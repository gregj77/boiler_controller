#include <Arduino.h>
#include <ArduinoLog.h>
#include "RuleEngine.h"
#include "HighProductionYieldRule.h"


HighProductionYieldRule::HighProductionYieldRule() noexcept {
}

void HighProductionYieldRule::onInit(ICommandDispatcher& dispatcher) {
    dispatcher.registerTimer(*this, 5000, true);
}

void HighProductionYieldRule::onStop(ICommandDispatcher& dispatcher) {
    // Ensure that the heat pump is turned off when the rule is stopped
    if (_isRequested) {
        _isRequested = false;
        dispatcher.dispatchCommand({CMD_RELAY_CTRL_OFF, getId()});
        Log.noticeln("[HIGH_PROD_RULE] Rule stopped, ensuring heat pump is turned off");
    }
}

void HighProductionYieldRule::processCommand(const Command& cmd, ICommandDispatcher& dispatcher) {
    bool dataChanged = false;
    switch (cmd.id) {
        case CMD_NOTIFY_ENERGY_BALANCE:
            _lastBalanceUpdate = millis();
            _isCommunicationOk = true;
            _currentBalance = static_cast<int32_t>(cmd.data);
            dataChanged = true;
            break;

        case CMD_NOTIFY_TEMP_TOP:
            _lastTemp = cmd.data / 100.0f;
            dataChanged = true;
            break;

        default:
            break;
    }    

    if (dataChanged) {
        checkActivationConditions(dispatcher);
    }
}

void HighProductionYieldRule::checkActivationConditions(ICommandDispatcher& dispatcher) {
    uint32_t now = millis();

        if (!_isRequested) {
            if (_currentBalance <= PV_PRODUCTION_THRESHOLD) {
                if (_productionStartTime == 0) {
                    _productionStartTime = now;
                    Log.verboseln("[HIGH_PROD_RULE] Detected potential high production (balance: %d W), starting confirmation timer", _currentBalance);
                }
                
                if ((now - _productionStartTime >= PRODUCTION_CONFIRMATION_TIME_MS) && (_lastTemp <= MAX_TEMP)) {
                    activate(dispatcher);
                }
            } else {
                // not enough production.... 
                _productionStartTime = 0;
            }
        } 
        else {
            bool tempReached = (_lastTemp >= MAX_TEMP);
            bool importTooHigh = (_currentBalance >= IMPORT_LIMIT_THRESHOLD);
            bool minTimePassed = (now - _activationTimestamp >= MIN_RUN_TIME_MS);

            if (minTimePassed && (tempReached || importTooHigh)) {
                deactivate(dispatcher, tempReached, importTooHigh);
            }
        }
}

void HighProductionYieldRule::onTimer(ICommandDispatcher& dispatcher) {
    uint32_t now = millis();

    // Sprawdzamy, czy nie minęło zbyt dużo czasu od ostatniej paczki danych
    if (_isRequested) {
        bool communicationLost = (now - _lastBalanceUpdate > WATCHDOG_TIMEOUT_MS);
        bool minTimePassed = (now - _activationTimestamp >= MIN_RUN_TIME_MS);

        if (communicationLost) {
            if (minTimePassed) {
                Log.errorln("[HIGH_PROD_RULE] WATCHDOG: Connection lost and min run time reached - STOP");
                _isRequested = false;
                _productionStartTime = 0;
                dispatcher.dispatchCommand({CMD_RELAY_CTRL_OFF, getId()});
            } 
        }
    }        
}

void HighProductionYieldRule::activate(ICommandDispatcher& dispatcher) {
    _isRequested = true;
    _activationTimestamp = millis();
    dispatcher.dispatchCommand({CMD_RELAY_CTRL_ON, getId()});
    Log.infoln("[HIGH_PROD_RULE] PV Surplus detected, starting HP");
}

void HighProductionYieldRule::deactivate(ICommandDispatcher& dispatcher, bool dueToTemp, bool dueToImport) {
    _isRequested = false;
    dispatcher.dispatchCommand({CMD_RELAY_CTRL_OFF, getId()});
    Log.noticeln("[HIGH_PROD_RULE] Stopping HP (Temp reached - %s, high import - %s)", getName(), dueToTemp ? "YES" : "NO", dueToImport ? "YES" : "NO");
}
