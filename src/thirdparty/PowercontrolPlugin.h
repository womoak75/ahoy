#ifndef __POWERCONTROLPLUGIN_H__
#define __POWERCONTROLPLUGIN_H__

#include "plugin.h"
#include "../hm/hmDefines.h"


class PowercontrolPlugin : public Plugin
{
    enum pluginIds {ACT_POWERLIMIT};

public:
    PowercontrolPlugin() : Plugin(3, "powercontrol") {}

    void setup() {}
    void loop() {
        if(update) {
            update = false;
            calcLimit();
        }
    }
    void calcLimit() {
        DPRINTLN(DBG_DEBUG, F("Powercontrol.calcLimit"));
        // TODO: do some magic calculation here
        // :/
        // float newlimit = magicFunction(actsupply,actconsumption);
        // publishLimit(newlimit);
    }
    void publishLimit(float l) {
        publishInternalValue(ValueEntry(ACT_POWERLIMIT,l));
        actlimit = l;
    }

    void internalDataCallback(PluginDataMessage *message) {
        if(message->has(PluginIds::PluginMeter,PluginMeterIds::ACPOWER_SUPPLY_ACTUAL)) {
            DPRINTLN(DBG_DEBUG, F("powercontroller got act supply"));
            actsupply = message->getFloatValueById(PluginMeterIds::ACPOWER_SUPPLY_ACTUAL);
            update = true;
        } else if(message->has(PluginIds::PluginInverter,PluginInverterIds::ACPOWER_TOTAL)) {
            DPRINTLN(DBG_DEBUG, F("powercontroller got act consumption"));
            actconsumption = message->getFloatValueById(PluginInverterIds::ACPOWER_TOTAL);
            update = true;
        }
    }

    private:
    float actsupply = 0.0;
    float actconsumption = 0.0;
    float actlimit = 0.0;
    bool update = false;
};

#endif /*__POWERCONTROLPLUGIN_H__*/