#pragma once

#include <IRule.h>

class BoilerController;

class MinTemperatureRule final : public IRule {
    private:
        BoilerController& _boilerController;
        float _threshold;
        bool _isTopTank;

    public:
        explicit MinTemperatureRule(BoilerController& boilerController) noexcept;

        MinTemperatureRule() = delete;
        MinTemperatureRule(const MinTemperatureRule&) = delete;
        MinTemperatureRule(MinTemperatureRule&&) = delete;
        MinTemperatureRule& operator=(const MinTemperatureRule&) = delete;
        MinTemperatureRule& operator=(MinTemperatureRule&&) = delete;

        const char* getName() const override { return "MinTemperature"; }
        uint16_t getId() const override { return 1; }

        void processCommand(const Command& cmd, ICommandDispatcher& dispatcher) override;
};