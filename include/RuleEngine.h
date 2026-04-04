#pragma once

#include <Arduino.h>
#include <vector>
#include <functional>
#include <memory>
#include <vector>
#include "IRule.h"
#include "ICommandDispatcher.h"
#include "RuleSetup.h"




class RuleEngine final : public ICommandDispatcher {
private:
    QueueHandle_t _commandQueue;
    static constexpr int CMD_READ_INTERVAL_MILLIS = 1000;

    struct RuleSlot {
        std::unique_ptr<IRule> rule; 
        bool isActive;

        uint32_t intervalMillis;
        uint32_t lastTick;
        bool isRecurring;
        bool isTimerActive;

        RuleSlot(std::unique_ptr<IRule> rule, bool active = true) 
            : rule(std::move(rule))
            , isActive(active)
            , intervalMillis(0)
            , lastTick(0)
            , isRecurring(false)
            , isTimerActive(false) {                
        }
    };

    std::vector<RuleSlot> _rules;
    uint32_t _lastTick;
    uint16_t _activeRulesMask;

    void onCheckTimers();
    void onProcessCommand();

public:
    explicit RuleEngine() noexcept;

    void init();

    void processCommands(); 

    void dispatchCommand(const Command& cmd) override;

    void registerTimer(IRule& sender, uint32_t intervalMillis, bool recurring) override;

    void registerRule(std::unique_ptr<IRule> rule, bool active);

    uint16_t getActiveRulesMask() const { return _activeRulesMask; };

    RuleEngine(const RuleEngine&) = delete;
    RuleEngine(RuleEngine&&) = delete;
    RuleEngine& operator=(const RuleEngine&) = delete;
    RuleEngine& operator=(RuleEngine&&) = delete;
};
