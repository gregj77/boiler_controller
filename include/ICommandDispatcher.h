#pragma once

struct Command;

class ICommandDispatcher {
    public:
        virtual ~ICommandDispatcher() = default;

        virtual void dispatchCommand(const Command& cmd) = 0;
};