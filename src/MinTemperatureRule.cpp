#include "MinTemperatureRule.h"
#include "BoilerController.h"
#include "RuleEngine.h"

MinTemperatureRule::MinTemperatureRule(BoilerController& boilerController) noexcept
: _boilerController(boilerController) {
}

void MinTemperatureRule::processCommand(const Command& cmd, ICommandDispatcher& dispatcher) {
    if (cmd.id == CMD_NOTIFY_TEMP_TOP) {
        _isTopTank = true;
        _threshold = cmd.data / 100.0f;
    }

}   