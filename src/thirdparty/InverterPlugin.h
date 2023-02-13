#ifndef __INVERTERPLUGIN_H__
#define __INVERTERPLUGIN_H__

#include "plugin.h"
#include "../hm/hmDefines.h"


class InverterPlugin : public Plugin
{
    enum pluginIds {ACPOWER_TOTAL};

public:
    InverterPlugin() : Plugin(1, "inverter") {
        inverterac.fill(0.0);
    }

    void setup() {}
    void loop() {
        if(updateInverter) {
            updateInverter = false;
            publishAC();
        }
        if(updateLimit) {
            updateLimit= false;
            calcLimit();
        }
    }

    void onTickerSetup() {

    }

    void setLimit(float limit) {
        DynamicJsonDocument json(64);
        JsonObject request = json.to<JsonObject>();
        // hack
        request[F("id")]=limit;
        request[F("path")]="ctrl";
        request[F("cmd")]="limit_nonpersistent_absolute";
        request[F("val")]=actlimit;
        sendCtrlRequest(request);
    }
    void calcLimit() {
        DPRINTLN(DBG_DEBUG, F("Inverter.calcLimit"));
        if(newlimit!=actlimit) {
            // if we got multiple inverters ... how to split the limit?
            setLimit(newlimit);
            // TODO: how to get response?
            // set actlimit=newlimit;
        }
    }

    void publishAC() {
        float sum = 0.0;
        for(int i = 0; i < MAX_NUM_INVERTERS; i++) {
            sum+=inverterac[i];
        }
        publishInternalValue(ValueEntry(ACPOWER_TOTAL,sum));
    }
    void inverterCallback(const InverterMessage *message)
    {
        if(message->fieldId==FLD_PAC) {
            inverterac[message->inverterId] = message->value;
            updateInverter = true;
        }
    }
    void internalDataCallback(PluginDataMessage *message) {
        if(message->has(PluginIds::PluginPowercontrol,PluginPowercontrolIds::ACT_POWERLIMIT)) {
            newlimit = message->getFloatValueById(PluginPowercontrolIds::ACT_POWERLIMIT);
            updateLimit = true;
        } 
    }
    private:
    std::array<float,MAX_NUM_INVERTERS> inverterac;
    float actlimit = 0.0;
    float newlimit = 0.0;
    bool updateLimit = false;
    bool updateInverter = false;
};

#endif /*__INVERTERPLUGIN_H__*/