#include <Arduino.h>
#include <ArduinoLog.h>
#include "LegionellaRule.h"
#include "ICommandDispatcher.h"
#include "RuleSetup.h"

LegionellaRule::LegionellaRule() noexcept {
}

void LegionellaRule::onInit(ICommandDispatcher& dispatcher) {
    dispatcher.registerTimer(*this, TIMER_CHECK_INTERVAL_MS, true); 
    _isWarmingUp = true;
    dispatcher.dispatchCommand({CMD_BOILER_75_DEGREES, getId()});
    Log.noticeln("[LEGIONELLA_RULE] Legionella warm-up start, requesting 75 °C");
}

void LegionellaRule::processCommand(const Command& cmd, ICommandDispatcher& dispatcher) {
    if (cmd.id != CMD_NOTIFY_TEMP_TOP) {
        return;
    }
    _lastTemp = cmd.data / 100.0f;


    if (_isWarmingUp) {
        if (_lastTemp >= 70.0f) {
            _isWarmingUp = false;
            _isRuleRunning = true;
            _sessionStartTime = millis();
            Log.noticeln("[LEGIONELLA_RULE] Legionella warm-up complete, temperature reached %F °C, starting timer....", _lastTemp);
        }
    } else if (_isRuleRunning) {
        if (_lastTemp <= 65.0f) {
            Log.warningln("[LEGIONELLA_RULE] Temperature dropped below safe threshold during legionella cycle (%F °C), restarting warm-up", _lastTemp);
            _isWarmingUp = true;
            _isRuleRunning = false;
            _sessionStartTime = 0;
        }
    }
}   

void LegionellaRule::onStop(ICommandDispatcher& dispatcher) {
    _isWarmingUp = false;
    _isRuleRunning = false;
    _sessionStartTime = 0;
    dispatcher.dispatchCommand({CMD_BOILER_OFF, getId()});
}
 
void LegionellaRule::onTimer(ICommandDispatcher& dispatcher) {
    if (_isWarmingUp && _lastTemp > 0.0f) {
        Log.verboseln("[LEGIONELLA_RULE] Warm-up in progress, current temperature: %F °C", _lastTemp);
    }
    if (_isRuleRunning && (millis() - _sessionStartTime >= LEGIONELLA_DURATION_MS)) {
        _isRuleRunning = false;
        dispatcher.dispatchCommand({CMD_BOILER_OFF, getId()});
        dispatcher.disableRule(*this);
        Log.noticeln("[LEGIONELLA_RULE] Legionella cycle complete, turning off boiler");
    }
}
