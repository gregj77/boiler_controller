#pragma once
#include <IRule.h>

class ICommandDispatcher;

class LegionellaRule final : public IRule {
    private:
        const uint32_t TIMER_CHECK_INTERVAL_MS = 10 * 1000; 
        const uint32_t LEGIONELLA_DURATION_MS = 15 * 60 * 1000; 

        bool _isWarmingUp = false;
        bool _isRuleRunning = false;
        uint32_t _sessionStartTime = 0;
        float _lastTemp = 0.0f;

    public:
        explicit LegionellaRule() noexcept;

        LegionellaRule(LegionellaRule&&) = delete;
        LegionellaRule& operator=(const LegionellaRule&) = delete;
        LegionellaRule& operator=(LegionellaRule&&) = delete;

        const char* getName() const override { return "Legionella"; }
        uint16_t getId() const override { return 8; }

        void onInit (ICommandDispatcher& dispatcher) override;
        void processCommand(const Command& cmd, ICommandDispatcher& dispatcher) override;
        void onStop(ICommandDispatcher& dispatcher) override;
        void onTimer(ICommandDispatcher& dispatcher) override;
};