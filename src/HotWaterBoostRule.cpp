#include <Arduino.h>
#include <ArduinoLog.h>
#include "HotWaterBoostRule.h"
#include "RuleSetup.h"
#include "ICommandDispatcher.h"

HotWaterBoostRule::HotWaterBoostRule() noexcept {
}

void HotWaterBoostRule::onInit(ICommandDispatcher& dispatcher) {      
    dispatcher.dispatchCommand({CMD_BOILER_65_DEGREES, getId()});
    Log.noticeln("[HOT_WATER_BOOST_RULE] Hot water boost activated, requesting 65 °C");
}

void HotWaterBoostRule::processCommand(const Command& cmd, ICommandDispatcher& dispatcher) {
    if (cmd.id != CMD_NOTIFY_TEMP_TOP) {
        return;
    }
    
    float currentTemp = cmd.data / 100.0f;
    if (currentTemp >= 65.0f) {
        dispatcher.dispatchCommand({CMD_BOILER_OFF, getId()});  
        dispatcher.disableRule(*this);
        Log.noticeln("[HOT_WATER_BOOST_RULE] Target temperature reached (%F °C), deactivating hot water boost", currentTemp);
    }
}   

void HotWaterBoostRule::onStop(ICommandDispatcher& dispatcher) {
    dispatcher.dispatchCommand({CMD_BOILER_OFF, getId()});  
}