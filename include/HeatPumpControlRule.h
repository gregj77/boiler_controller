#pragma once

#include <IRule.h>
#include <vector>

class BoilerController;
class ICommandDispatcher;

class HeatPumpControlRule final : public IRule {
    private:
        BoilerController& _boilerController;

        struct HeatPumpRequest {
            uint16_t ruleId;
            bool operator==(uint16_t id) const { return ruleId == id; }
        };

        std::vector<HeatPumpRequest> _activeRequests;

        void addHeatPumpRequest(uint16_t ruleId);
        void removeHeatPumpRequest(uint16_t ruleId);

    public:
        explicit HeatPumpControlRule(BoilerController& boilerController) noexcept;

        HeatPumpControlRule(const HeatPumpControlRule&) = delete;
        HeatPumpControlRule(HeatPumpControlRule&&) = delete;
        HeatPumpControlRule& operator=(const HeatPumpControlRule&) = delete;
        HeatPumpControlRule& operator=(HeatPumpControlRule&&) = delete;

        const char* getName() const override { return "HeatPumpControl"; }
        uint16_t getId() const override { return 16; }

        void processCommand(const Command& cmd, ICommandDispatcher& dispatcher) override;
        void onStop(ICommandDispatcher& dispatcher) override;
};