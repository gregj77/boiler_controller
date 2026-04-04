#pragma once

#include <Arduino.h>

struct Command;
class ICommandDispatcher;

class IRule {
    public:
        virtual ~IRule() = default;

        virtual uint16_t getId() const = 0;

        virtual void init (ICommandDispatcher& dispatcher) {};

        virtual void stop(ICommandDispatcher& dispatcher) {};

        virtual void processCommand(const Command& cmd, ICommandDispatcher& dispatcher) = 0;

        virtual void onTimer(ICommandDispatcher& dispatcher) {};

        virtual const char* getName() const = 0;
};