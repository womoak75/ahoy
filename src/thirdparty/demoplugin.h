#ifndef __DEMOPLUGIN_H__
#define __DEMOPLUGIN_H__

#include "plugin.h"

class demoPlugin : public Plugin
{
public:
    demoPlugin() : Plugin(999,"demo") {}
    demoPlugin(int id, const char* name) : Plugin(id,name) {}
    void setup() {
        addTimerCb(SECOND,[this](){
            PluginMessage msg;
            msg.valuename = "SOMEPLUGINOUTPUT";
            msg.value = 42;
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
          // send mqtt message to 'DEF_MQTT_TOPIC/demoplugin/out'
          // default for DEF_MQTT_TOPIC = "inverter" (see config.h)
          mqttMsg.appendTopic = true;
          enqueueMessage(&mqttMsg);
          // send mqtt message to 'demoplugin/out'
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
         DPRINTLN(DBG_INFO, F("demoplugin.mqttCallback ")+String(message->topic));
      }

     void internalCallback(PluginMessage *message) {
        DPRINTLN(DBG_INFO, F("demoplugin.internalCallback: ")+String(message->valuename)+String(" = ")+String(message->value));
     }
    MqttMessage mqttMsg;
};

#endif /*__DEMOPLUGIN_H__*/