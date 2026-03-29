#pragma once

#include <Arduino.h>

struct Command;
class ICommandDispatcher;

class IRule {
    public:
        virtual ~IRule() = default;

        virtual uint16_t getId() const = 0;

        virtual void processCommand(const Command& cmd, ICommandDispatcher& dispatcher) = 0;

        virtual const char* getName() const = 0;
};