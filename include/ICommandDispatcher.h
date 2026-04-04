#pragma once

struct Command;
class IRule;

class ICommandDispatcher {
    public:
        virtual ~ICommandDispatcher() = default;

        virtual void dispatchCommand(const Command& cmd) = 0;

        virtual void registerTimer(IRule& sender, uint32_t intervalMillis, bool recurring) = 0;
};