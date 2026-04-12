#pragma once    


#pragma once

#include <IRule.h>


class LowTemperatureBoostRule final : public IRule {
    private:
        constexpr static float MIN_TEMP_THRESHOLD = 30.0f;
        constexpr static float EXPECTED_TEMP = 36.0f;

        bool _isActive;
        float _currentTemperature;

    public:
        explicit LowTemperatureBoostRule() noexcept;

        LowTemperatureBoostRule(const LowTemperatureBoostRule&) = delete;
        LowTemperatureBoostRule(LowTemperatureBoostRule&&) = delete;
        LowTemperatureBoostRule& operator=(const LowTemperatureBoostRule&) = delete;
        LowTemperatureBoostRule& operator=(LowTemperatureBoostRule&&) = delete;

        const char* getName() const override { return "LowTemperatureBoost"; }
        uint16_t getId() const override { return 16; }

        void processCommand(const Command& cmd, ICommandDispatcher& dispatcher) override;
        void onStop(ICommandDispatcher& dispatcher) override;
};