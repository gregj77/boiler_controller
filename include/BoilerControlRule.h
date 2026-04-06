#pragma once

#include <IRule.h>
#include <vector>
#include "RuleSetup.h"

class BoilerController;
class ICommandDispatcher;

class BoilerControlRule final : public IRule {
    private:
        static constexpr float IDLE_TEMP = static_cast<float>(CommandID::CMD_BOILER_OFF);
        static constexpr float RAMP_STEP = 0.2f;            // 0.2°C per second
        static constexpr uint32_t SENSOR_TIMEOUT_MS = 5 * 60 * 1000; // 5 minutes
        static constexpr uint32_t EFFICIENCY_TIMEOUT_MS = 15 * 60 * 1000; // 15 minutes

        BoilerController& _boilerController;

        struct BoilerTempRequest {
            uint16_t ruleId;
            uint8_t temperature;
            bool operator==(uint16_t id) const { return ruleId == id; }
        };

        std::vector<BoilerTempRequest> _activeRequests;
        float _targetTemperature;
        float _currentOutputTemperature;

        uint32_t _lastSensorUpdate;
        uint32_t _heatingStartTime;

        float _lastMeasuredTemp;
        float _tempAtStartOfHeating;

        void calculateTarget();
        void stop(const char* reason);

    public:
        explicit BoilerControlRule(BoilerController& boilerController) noexcept;

        BoilerControlRule(const BoilerControlRule&) = delete;
        BoilerControlRule(BoilerControlRule&&) = delete;
        BoilerControlRule& operator=(const BoilerControlRule&) = delete;
        BoilerControlRule& operator=(BoilerControlRule&&) = delete;

        const char* getName() const override { return "BoilerControl"; }
        uint16_t getId() const override { return 32; }

        void onInit (ICommandDispatcher& dispatcher) override;
        void processCommand(const Command& cmd, ICommandDispatcher& dispatcher) override;
        void onTimer(ICommandDispatcher& dispatcher) override;
        void onStop(ICommandDispatcher& dispatcher) override;
};