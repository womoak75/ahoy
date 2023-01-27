#ifndef __DEMOPLUGIN2_H__
#define __DEMOPLUGIN2_H__

#include "plugin.h"

class demoPlugin2 : public Plugin
{
public:
    demoPlugin2() : Plugin(99,"demo2") {}
    demoPlugin2(int id, const char* name) : Plugin(id,name) {}

    void setup() {
        addTimerCb(SECOND,[this](){
            PluginMessage msg;
            msg.valuename = "SOMEOTHERPLUGINOUTPUT";
            msg.value = 23;
            publishInternal(&msg);
        });
    }
    void loop() {
        // main loop
        mqttMsg.topic = "out";
        mqttMsg.payload = (uint8_t*)"ahoi world!";
        mqttMsg.length = 11;
        static unsigned long last = 0;
        if((millis()-last)>=5000) {
          last = millis();
          // send mqtt message to 'DEF_MQTT_TOPIC/demoplugin2/out'
          // default for DEF_MQTT_TOPIC = "inverter" (see config.h)
          mqttMsg.appendTopic = true;
          enqueueMessage(&mqttMsg);
          // send mqtt message to 'demoplugin2/out'
          mqttMsg.appendTopic=false;
          enqueueMessage(&mqttMsg);
        }
     }
     void inverterCallback(const InverterMessage *message) {
        // receice inverter data
     }
     void mqttCallback(const MqttMessage *message) {
         // receive data for
         // ahoi topic: 'DEF_MQTT_TOPIC/devcontrol/#'
         // thirdparty topic: 'DEF_MQTT_TOPIC/thirdparty/#'
         // default for DEF_MQTT_TOPIC = "inverter" (see config.h)
      }

     void internalCallback(PluginMessage *message) {
         // internal topic: '{pluginname}/{dataidentifier}'
        DPRINTLN(DBG_INFO, F("demoplugin2.internalCallback: ")+String(message->valuename)+String(" = ")+String(message->value));
     }
     MqttMessage mqttMsg;
};

#endif /*__DEMOPLUGIN2_H__*/