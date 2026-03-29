#pragma once

#include <Arduino.h>
#include <vector>
#include <functional>
#include <memory>
#include <vector>
#include "IRule.h"
#include "ICommandDispatcher.h"

struct Command {
    uint16_t id;
    int32_t data;
};

namespace CommandRange {
    const uint16_t CONTROL_START = 1;
    const uint16_t NOTIFY_START  = 100;
    const uint16_t CONFIG_START  = 200;
    const uint16_t SYSTEM_START  = 250;
}

enum CommandID : uint16_t {
    CMD_SET_RULES_MASK    = 1,
    CMD_MANUAL_RELAY_CTRL = 3,

    CMD_NOTIFY_TEMP_TOP    = 100,
    CMD_NOTIFY_TEMP_BOTTOM = 101,

    CMD_SYS_RESET          = 250,
};


class RuleEngine final : public ICommandDispatcher {
private:
    QueueHandle_t _commandQueue;
    static constexpr int CMD_READ_INTERVAL_MILLIS = 1000;

    struct RuleSlot {
        std::unique_ptr<IRule> rule; 
        bool isActive;

        RuleSlot(std::unique_ptr<IRule> rule, bool active = true) : rule(std::move(rule)), isActive(active) {}
    };

    std::vector<RuleSlot> _rules;

public:
    explicit RuleEngine() noexcept;

    void processCommands(); 

    void dispatchCommand(const Command& cmd) override;

    void registerRule(std::unique_ptr<IRule> rule, bool active);

    RuleEngine(const RuleEngine&) = delete;
    RuleEngine(RuleEngine&&) = delete;
    RuleEngine& operator=(const RuleEngine&) = delete;
    RuleEngine& operator=(RuleEngine&&) = delete;
};
