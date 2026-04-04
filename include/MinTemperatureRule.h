#pragma once

#include <IRule.h>


class MinTemperatureRule final : public IRule {
    private:
        constexpr static float MIN_TEMP_THRESHOLD = 35.0f;
        constexpr static float EXPECTED_TEMP = 45.0f;

        bool _isActive;
        float _currentTemperature;

    public:
        explicit MinTemperatureRule() noexcept;

        MinTemperatureRule(const MinTemperatureRule&) = delete;
        MinTemperatureRule(MinTemperatureRule&&) = delete;
        MinTemperatureRule& operator=(const MinTemperatureRule&) = delete;
        MinTemperatureRule& operator=(MinTemperatureRule&&) = delete;

        const char* getName() const override { return "MinTemperature"; }
        uint16_t getId() const override { return 1; }

        void processCommand(const Command& cmd, ICommandDispatcher& dispatcher) override;
        void stop(ICommandDispatcher& dispatcher) override;
};