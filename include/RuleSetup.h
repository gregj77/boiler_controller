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
    CMD_RELAY_CTRL_ON     = 10,
    CMD_RELAY_CTRL_OFF    = 11,

    CMD_BOILER_25_DEGREES   = 25,
    CMD_BOILER_35_DEGREES   = 35,
    CMD_BOILER_45_DEGREES   = 45,
    CMD_BOILER_55_DEGREES   = 55,  
    CMD_BOILER_65_DEGREES   = 65,
    CMD_BOILER_75_DEGREES   = 75,
    CMD_BOILER_OFF          = 90,

    CMD_NOTIFY_TEMP_TOP         = 100,
    CMD_NOTIFY_TEMP_BOTTOM      = 101,
    CMD_NOTIFY_ENERGY_BALANCE   = 102,

    CMD_TOP_SENSOR_ERROR        = 201,
    CMD_BOTTOM_SENSOR_ERROR     = 202,
};
