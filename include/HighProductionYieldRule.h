#pragma once

#include <IRule.h>


class HighProductionYieldRule final : public IRule {
    private:

        static constexpr float MAX_TEMP = 58.0f;
        static constexpr int32_t PV_PRODUCTION_THRESHOLD = -700; // -700W (energy production)
        static constexpr int32_t IMPORT_LIMIT_THRESHOLD = 1000;  // 1000W (energy consumption)
        static constexpr uint32_t MIN_RUN_TIME_MS = 30 * 60 * 1000; // 30 minutes
        static constexpr uint32_t PRODUCTION_CONFIRMATION_TIME_MS = 5 * 60 * 1000; // 5 minutes
        static constexpr uint32_t WATCHDOG_TIMEOUT_MS = 3 * 60 * 1000; // 3 minutes

        float _lastTemp = 0.0f;
        int32_t _currentBalance = 0;
        bool _isRequested = false;
        uint32_t _activationTimestamp = 0;
        uint32_t _productionStartTime = 0;

        uint32_t _lastBalanceUpdate = 0;
        bool _isCommunicationOk = false;

        void checkActivationConditions(ICommandDispatcher& dispatcher);

        void activate(ICommandDispatcher& dispatcher);
        void deactivate(ICommandDispatcher& dispatcher, bool dueToTemp, bool dueToImport);

    public:
        explicit HighProductionYieldRule() noexcept;

        HighProductionYieldRule(const HighProductionYieldRule&) = delete;
        HighProductionYieldRule(HighProductionYieldRule&&) = delete;
        HighProductionYieldRule& operator=(const HighProductionYieldRule&) = delete;
        HighProductionYieldRule& operator=(HighProductionYieldRule&&) = delete;

        const char* getName() const override { return "HighProductionYield"; }
        uint16_t getId() const override { return 2; }

        void init (ICommandDispatcher& dispatcher) override;
        void stop(ICommandDispatcher& dispatcher) override;
        void processCommand(const Command& cmd, ICommandDispatcher& dispatcher) override;
        void onTimer(ICommandDispatcher& dispatcher) override;
};