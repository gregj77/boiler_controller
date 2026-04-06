#include "RuleEngine.h"
#include <ArduinoLog.h>
#include <projdefs.h>

RuleEngine::RuleEngine() noexcept {
    _commandQueue = xQueueCreate(20, sizeof(Command));
}

void RuleEngine::init() {
    uint16_t activeRuleMask = 0;

    for (auto& slot : _rules) {
        if (slot.isActive) {
            Log.noticeln("[RULE_ENGINE] Initializing rule '%s'", slot.rule->getName());
            slot.rule->onInit(*this);
            activeRuleMask |= slot.rule->getId();
        }
    }
    _activeRulesMask = activeRuleMask;
    Log.noticeln("[RULE_ENGINE] Rule engine initialized with %d rules", _rules.size());
}

void RuleEngine::dispatchCommand(const Command& cmd) {
    if (xQueueSend(_commandQueue, &cmd, 0) == errQUEUE_FULL) {
        Log.warningln("[RULE_ENGINE] Command queue full, failed to send command ID: %d", cmd.id);
    }
}

void RuleEngine::registerRule(std::unique_ptr<IRule> rule, bool active) {
    _rules.emplace_back(std::move(rule), active);
}

void RuleEngine::disableRule(IRule& rule) {
    for (auto& slot : _rules) {
        if (slot.rule.get() == &rule) {
            if (slot.isActive) {
                slot.isActive = false;
                slot.isTimerActive = false;
                slot.rule->onStop(*this);
                Log.noticeln("[RULE_ENGINE] Rule '%s' disabled", rule.getName());
            }
            return;
        }
    }
    Log.warningln("[RULE_ENGINE] Attempted to disable unknown rule '%s'", rule.getName());
}

void RuleEngine::registerTimer(IRule& sender, uint32_t intervalMillis, bool recurring) {
    for (auto& slot : _rules) {
        if (slot.rule.get() == &sender) {
            slot.lastTick = millis();
            slot.intervalMillis = intervalMillis;
            slot.isRecurring = recurring;
            slot.isTimerActive = intervalMillis != 0;
            Log.noticeln("[RULE_ENGINE] Registered timer for rule '%s' with interval %d ms", sender.getName(), intervalMillis);
            return;
        }
    }
    Log.warningln("[RULE_ENGINE] Attempted to register timer for unknown rule '%s'", sender.getName());
}

void RuleEngine::processCommands() {
    onCheckTimers();
    onProcessCommand();
}

void RuleEngine::onCheckTimers() {
    uint32_t now = millis();
    if (now - _lastTick < CMD_READ_INTERVAL_MILLIS) {
        return;
    }

    _lastTick = now;
    for (auto& slot : _rules) {
        if (!slot.isTimerActive || !slot.isActive) {
            continue;
        }

        if (now - slot.lastTick >= slot.intervalMillis) {

            if (slot.isRecurring) {
                slot.lastTick += slot.intervalMillis;
                if (now - slot.lastTick >= slot.intervalMillis) {
                    slot.lastTick = now;
                }
            } else {
                slot.isTimerActive = false;
            }
        
            Log.verboseln("[RULE_ENGINE] Timer triggered for rule '%s'", slot.rule->getName());
            slot.rule->onTimer(*this);
        }
    }        
}

void RuleEngine::onProcessCommand() {
    Command cmd;
    static TickType_t delay = pdMS_TO_TICKS(CMD_READ_INTERVAL_MILLIS);
    if (xQueueReceive(_commandQueue, &cmd, delay) == pdTRUE) {
        Log.noticeln("[RULE_ENGINE] Processing command ID: %d, Data: %d", cmd.id, cmd.data);
        CommandID id = static_cast<CommandID>(cmd.id);

        switch (id) {
            case CMD_SET_RULES_MASK: {
                uint16_t mask = static_cast<uint16_t>(cmd.data);
                Log.infoln("[RULE_ENGINE] Updating rules active mask to %d", mask);
                uint16_t activeRuleMask = 0;
                for (auto& slot : _rules) {
                    bool shouldBeActivated = (slot.rule->getId() & mask) != 0;

                    bool isActive = slot.isActive;
                    if (shouldBeActivated && !isActive) {
                        slot.isActive = true;
                        slot.rule->onInit(*this);
                        activeRuleMask |= slot.rule->getId();
                        Log.noticeln("[RULE_ENGINE] Activating rule '%s' due to mask update", slot.rule->getName());
                    }

                    if (isActive && !shouldBeActivated) {
                        slot.isActive = false;
                        slot.isTimerActive = false; 
                        slot.rule->onStop(*this);
                        Log.noticeln("[RULE_ENGINE] Deactivating rule '%s' due to mask update", slot.rule->getName());
                    }
                }
                _activeRulesMask = activeRuleMask;            
                break;
            }

            default:
                for (auto& slot : _rules) {
                    if (slot.isActive) {
                        slot.rule->processCommand(cmd, *this);
                    }
                }
                break;
        }
    }
}