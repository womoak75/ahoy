#ifndef __DEMOPLUGIN_H__
#define __DEMOPLUGIN_H__

#include "plugin.h"

enum demoPluginIds {SOMEVALUE,SOMEOTHERVALUE};

class demoPlugin : public Plugin
{
public:
    demoPlugin() : Plugin(999, "demo") {}
    void setup()
    {

    }
    void onTickerSetup() {
        addTimerCb(SECOND, 3, [this]() { 
            publishInternalValues({ValueEntry(SOMEVALUE, (float)42.0),ValueEntry(SOMEOTHERVALUE, "blah blub")});            
        },"demoplugintimer1");
        addTimerCb(SECOND, 4, [this]() {
            enqueueMessage((char*)"out",(char*)"ahoi world!",false);
            enqueueMessage((char*)"out",(char*)"ahoi world!"); 
        },"demoplugintimer2");
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
        char buffer[64];
        for(int index = 0 ; index < message->getValueEntryCount(); index++) {
            if(message->isBoolValue(index))
                snprintf(buffer,sizeof(buffer),"Plugin:%d,Valueid:%d,Value:%d",message->getPluginId(),message->getValueId(index),message->getBoolValue(index));
            else if(message->isFloatValue(index))
                snprintf(buffer,sizeof(buffer),"Plugin:%d,Valueid:%d,Value:%f",message->getPluginId(),message->getValueId(index),message->getFloatValue(index));
            else
                snprintf(buffer,sizeof(buffer),"Plugin:%d,Valueid:%d,Value:%s",message->getPluginId(),message->getValueId(index),message->getCharValue(index));
            DPRINTLN(DBG_INFO,buffer);
        }
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