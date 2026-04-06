#include "IRule.h"

class ICommandDispatcher;

class HotWaterBoostRule final : public IRule {
    public:
        explicit HotWaterBoostRule() noexcept;

        HotWaterBoostRule(HotWaterBoostRule&&) = delete;
        HotWaterBoostRule& operator=(const HotWaterBoostRule&) = delete;
        HotWaterBoostRule& operator=(HotWaterBoostRule&&) = delete;

        const char* getName() const override { return "HotWaterBoost"; }
        uint16_t getId() const override { return 4; }

        void onInit (ICommandDispatcher& dispatcher) override;
        void processCommand(const Command& cmd, ICommandDispatcher& dispatcher) override;
        void onStop(ICommandDispatcher& dispatcher) override;
};