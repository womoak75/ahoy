#ifndef __DEMOPLUGIN2_H__
#define __DEMOPLUGIN2_H__

#include "plugin.h"

class demoPlugin2 : public Plugin
{
public:
    demoPlugin2() : Plugin(99,"demo2") {}
    demoPlugin2(int id, const char* name) : Plugin(id,name) {}

    void setup() {
        addTimerCb(SECOND,7,[this](){
            publishInternalValue("SOMEOTHERPLUGINOUTPUT",23);
        });
        addTimerCb(SECOND, 4, [this]() {
            enqueueMessage((char*)"out",(char*)"hello world!",false);
            enqueueMessage((char*)"out",(char*)"ahoi world!"); 
        });
    }
    void onMqttSubscribe() {
        subscribeMqtt((char*)"external/topic",false);
    }
    void loop() {
        // main loop
     }
     void inverterCallback(const InverterMessage *message) {
        // receice inverter data
     }
     void mqttCallback(const MqttMessage *message) {
         // receive data for
         // ahoi topic: 'DEF_MQTT_TOPIC/devcontrol/#'
         // thirdparty topic: 'DEF_MQTT_TOPIC/thirdparty/#'
         // default for DEF_MQTT_TOPIC = "inverter" (see config.h)
         DPRINTLN(DBG_INFO, F("demoplugin2.mqttCallback ") + String(message->topic));
      }

     void internalCallback(const PluginMessage *message) {
         // internal topic: '{pluginname}/{dataidentifier}'
        DPRINTLN(DBG_INFO, F("demoplugin2.internalCallback: ")+String(message->valuename)+String(" = ")+String(message->value));
     }
     MqttMessage mqttMsg;
};

#endif /*__DEMOPLUGIN2_H__*/