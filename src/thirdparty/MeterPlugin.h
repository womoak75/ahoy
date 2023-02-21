#ifndef __METERPLUGIN_H__
#define __METERPLUGIN_H__

#include "plugin.h"
#include "../hm/hmDefines.h"


class MeterPlugin : public Plugin
{
    enum pluginIds {ACPOWER_SUPPLY_ACTUAL};

public:
    MeterPlugin() : Plugin(2, "meter") {}

    void setup() {}
    void loop() {
        if(update) {
            update = false;
            publishAC();
        }
    }

    bool onRequest(JsonObject request, JsonObject response) { 
        // {'pluginname':'meter','acsupply':280}
        if(request.containsKey(F("acsupply"))) {
            acsupplyact = request[F("acsupply")];
            update = true;
        }
        return true; 
    }

    void publishAC() {
        DPRINTLN(DBG_DEBUG, F("meter.publishAC"));
        PluginMessage m(*this);
        m.add(FloatValue(ACPOWER_SUPPLY_ACTUAL,acsupplyact));
        publishMessage(m);
    }

    private:
    float acsupplyact = 0.0;
    bool update = false;
};

#endif /*__METERPLUGIN_H__*/