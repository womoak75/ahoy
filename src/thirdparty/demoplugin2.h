#ifndef __DEMOPLUGIN2_H__
#define __DEMOPLUGIN2_H__

#include "plugin.h"

enum demoPlugin2Ids {SOMEOTHERPLUGINOUTPUT};

class demoPlugin2 : public Plugin
{
public:
    demoPlugin2() : Plugin(99,"demo2") {}

    void setup() {

    }
    void onTickerSetup() {
        addTimerCb(SECOND,7,[this](){
            bool val = (millis()%2==0)?true:false;
            publishInternalValue(ValueEntry(SOMEOTHERPLUGINOUTPUT,val));
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

    void internalCallback(const PluginMessage *message) {
         // internal topic: '{pluginname}/{dataidentifier}'
        char buffer[64];
        for(int index = 0 ; index < message->getValueEntryCount(); index++) {
            if(message->isBoolValue(index))
                snprintf(buffer,sizeof(buffer),"Plugin:%d,Valueid:%d,Bool:%d",message->getPluginId(),message->getValueId(index),message->getBoolValue(index));
            else if(message->isFloatValue(index))
                snprintf(buffer,sizeof(buffer),"Plugin:%d,Valueid:%d,Float:%f",message->getPluginId(),message->getValueId(index),message->getFloatValue(index));
            else
                snprintf(buffer,sizeof(buffer),"Plugin:%d,Valueid:%d,Char:%s",message->getPluginId(),message->getValueId(index),message->getCharValue(index));
            DPRINTLN(DBG_INFO,buffer);
        }
    }
     MqttMessage mqttMsg;
};

#endif /*__DEMOPLUGIN2_H__*/