#include "RuleEngine.h"
#include <ArduinoLog.h>
#include <projdefs.h>

RuleEngine::RuleEngine() noexcept {
    _commandQueue = xQueueCreate(20, sizeof(Command));
}

void RuleEngine::dispatchCommand(const Command& cmd) {
    if (xQueueSend(_commandQueue, &cmd, 0) == errQUEUE_FULL) {
        Log.warningln("[RULE_ENGINE] Command queue full, failed to send command ID: %d", cmd.id);
    }
}

void RuleEngine::registerRule(std::unique_ptr<IRule> rule, bool active) {
    _rules.emplace_back(std::move(rule), active);
}

void RuleEngine::processCommands() {
    Command cmd;
    static TickType_t delay = pdMS_TO_TICKS(CMD_READ_INTERVAL_MILLIS);
    if (xQueueReceive(_commandQueue, &cmd, delay) == pdTRUE) {
        Log.noticeln("[RULE_ENGINE] Processing command ID: %d, Data: %d", cmd.id, cmd.data);
        CommandID id = static_cast<CommandID>(cmd.id);
        switch (id)
        {
        case CMD_SET_RULES_MASK:
            break;

        case CMD_NOTIFY_TEMP_BOTTOM:
            break;

        case CMD_NOTIFY_TEMP_TOP:
            break;
        
        default:
            break;
        }
    }
}