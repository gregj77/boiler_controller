#pragma once

#include <Arduino.h>

struct Command {
    uint16_t id;
    int32_t data;
};

namespace CommandRange {
    const uint16_t CONTROL_START = 1;
    const uint16_t NOTIFY_START  = 100;
    const uint16_t CONFIG_START  = 200;
    const uint16_t SYSTEM_START  = 250;
}

enum CommandID : uint16_t {
    CMD_SET_RULES_MASK    = 1,
    CMD_RELAY_CTRL_ON     = 50,
    CMD_RELAY_CTRL_OFF    = 51,


    CMD_NOTIFY_TEMP_TOP         = 100,
    CMD_NOTIFY_TEMP_BOTTOM      = 101,
    CMD_NOTIFY_ENERGY_BALANCE   = 102,

    CMD_SYS_RESET          = 250,
};
