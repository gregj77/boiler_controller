#pragma once

#include <IRule.h>
#include <vector>

class BoilerController;
class ICommandDispatcher;

class BoilerControlRule final : public IRule {
    private:
        BoilerController& _boilerController;

        struct HeatPumptRequest {
            uint16_t ruleId;
            bool operator==(uint16_t id) const { return ruleId == id; }
        };

        std::vector<HeatPumptRequest> _activeRequests;

        void addHeatPumpRequest(uint16_t ruleId);
        void removeHeatPumpRequest(uint16_t ruleId);

    public:
        explicit BoilerControlRule(BoilerController& boilerController) noexcept;

        BoilerControlRule(const BoilerControlRule&) = delete;
        BoilerControlRule(BoilerControlRule&&) = delete;
        BoilerControlRule& operator=(const BoilerControlRule&) = delete;
        BoilerControlRule& operator=(BoilerControlRule&&) = delete;

        const char* getName() const override { return "BoilerControl"; }
        uint16_t getId() const override { return 1 << 15; }

        void processCommand(const Command& cmd, ICommandDispatcher& dispatcher) override;
        void stop(ICommandDispatcher& dispatcher) override;
};