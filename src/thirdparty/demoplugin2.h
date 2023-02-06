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
            publishInternalBoolValue(SOMEOTHERPLUGINOUTPUT,(millis()%2==0)?true:false);
        },"demoplugin2timer1");
        addTimerCb(SECOND, 4, [this]() {
            enqueueMessage((char*)"out",(char*)"hello world!",false);
            enqueueMessage((char*)"out",(char*)"ahoi world!"); 
        },"demoplug2intimer2");
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
        if(message->isBoolValue())
            snprintf(buffer,sizeof(buffer),"Plugin:%d,Valueid:%d,Value:%d",message->getPluginId(),message->getValueId(),message->getBoolValue());
        else if(message->isFloatValue())
            snprintf(buffer,sizeof(buffer),"Plugin:%d,Valueid:%d,Value:%f",message->getPluginId(),message->getValueId(),message->getFloatValue());
        else
            snprintf(buffer,sizeof(buffer),"Plugin:%d,Valueid:%d,Value:%s",message->getPluginId(),message->getValueId(),message->getCharValue());
        DPRINTLN(DBG_INFO,buffer);
    }
     MqttMessage mqttMsg;
};

#endif /*__DEMOPLUGIN2_H__*/