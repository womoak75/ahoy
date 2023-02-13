#ifndef __DEMOPLUGIN_H__
#define __DEMOPLUGIN_H__

#include "plugin.h"



class demoPlugin : public Plugin
{
    enum demoPluginIds {SOMEVALUE,SOMEOTHERVALUE};

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
        if(debugHeap) {
            addTimerCb(SECOND, 10, [this]() {
                DPRINTLN(DBG_INFO, F("free heap: ") + String(ESP.getFreeHeap()));
            },"debugHeapTimer");
        }
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

    void internalDataCallback(PluginDataMessage *message)
    {   
        if(debugPluginMessages) {
            DBGPRINTMESSAGELN(DBG_INFO,message);
        }
    }

    bool onRequest(JsonObject request, JsonObject response) { 
        response[F("someoutput")]=millis();
        return true; 
    }

    void saveSettings(JsonObject settings) {
        settings[F("debugHeap")]=debugHeap;
        settings[F("debugPluginMessages")]=debugPluginMessages;
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
        if(settings.containsKey(F("debugHeap")))
            debugHeap=settings[F("debugHeap")];
         if(settings.containsKey(F("debugPluginMessages")))
            debugPluginMessages=settings[F("debugPluginMessages")];
    }
    private:
    bool debugHeap = false;
    bool debugPluginMessages = false;
    bool booleansetting = false;
    float floatsetting = 23.0;
    char stringsetting[32] = "some default string";
};

#endif /*__DEMOPLUGIN_H__*/