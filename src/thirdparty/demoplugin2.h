#ifndef __DEMOPLUGIN2_H__
#define __DEMOPLUGIN2_H__

#include "plugin.h"

class demoPlugin2 : public Plugin
{
    enum demoPlugin2Ids {SOMEOTHERPLUGINOUTPUT};
public:
    demoPlugin2() : Plugin(99,"demo2") {}

    void setup() {

    }
    void onTickerSetup() {
        addTimerCb(SECOND,7,[this](){
            int val = (int)millis();
            PluginMessage m(*this);
            m.add(IntValue(SOMEOTHERPLUGINOUTPUT,val));
            publishMessage(m);
        },"demoplugin2timer1");
        addTimerCb(SECOND, 4, [this]() {
            enqueueMessage((char*)"out",(char*)"hello world!",false);
            enqueueMessage((char*)"out",(char*)"ahoi world!"); 
        },"demoplug2intimer2");
        addTimerCb(SECOND,10,[this]{
            DPRINTLN(DBG_INFO, F("free heap: ") + String(system_get_free_heap_size()));
        },"heapcheck");
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

    void internalDataCallback(PluginMessage *message) {
         // internal topic: '{pluginname}/{dataidentifier}'
        if(!message->from(PluginIds::PluginDemo))
            return;
        DBGPRINTMESSAGELN(DBG_INFO,message);
    }
     MqttMessage mqttMsg;
};

#endif /*__DEMOPLUGIN2_H__*/