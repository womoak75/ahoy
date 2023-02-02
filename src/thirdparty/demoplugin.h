#ifndef __DEMOPLUGIN_H__
#define __DEMOPLUGIN_H__

#include "plugin.h"

class demoPlugin : public Plugin
{
public:
    demoPlugin() : Plugin(999, "demo") {}
    demoPlugin(int id, const char *name) : Plugin(id, name) {}
    void setup()
    {
        addTimerCb(SECOND, 3, [this]() { 
            publishInternalValue("SOMEPLUGINOUTPUT", 42); 
        });
        addTimerCb(SECOND, 4, [this]() {
            enqueueMessage((char*)"out",(char*)"ahoi world!",false);
            enqueueMessage((char*)"out",(char*)"ahoi world!"); 
        });
    }
    void loop()
    {
        // main loop
    }
    void inverterCallback(const InverterMessage *message)
    {
        // receice inverter data
    }
    void mqttCallback(const MqttMessage *message)
    {
        // receive data for
        // ahoi topic: 'DEF_MQTT_TOPIC/devcontrol/#'
        // thirdparty topic: 'DEF_MQTT_TOPIC/thirdparty/#'
        // default for DEF_MQTT_TOPIC = "inverter" (see config.h)
        DPRINTLN(DBG_INFO, F("demoplugin.mqttCallback ") + String(message->topic)+String(" = ")+String((char*)message->payload));
    }

    void internalCallback(const PluginMessage *message)
    {
        DPRINTLN(DBG_INFO, F("demoplugin.internalCallback: ") + String(message->valuename) + String(" = ") + String(message->value));
    }
    bool onRequest(JsonObject request, JsonObject response) { 
        response[F("someoutput")]=millis();
        return true; 
    }

    void saveSettings(JsonObject settings) {
        settings[F("booleansetting")]=booleansetting;
        settings[F("floatsetting")]=floatsetting;
        settings[F("stringsetting")]=stringsetting;
    }
    void loadSettings(JsonObject settings) {
        if(settings.containsKey(F("booleansetting")))
            booleansetting=settings[F("booleansetting")];
        if(settings.containsKey(F("floatsetting")))
            floatsetting=settings[F("floatsetting")];
        if(settings.containsKey(F("stringsetting")))
            settings[F("stringsetting")].as<String>().toCharArray(stringsetting,sizeof(stringsetting));
    }
    private:
    bool booleansetting = false;
    float floatsetting = 23.0;
    char stringsetting[32] = "some default string";
};

#endif /*__DEMOPLUGIN_H__*/