#ifndef __DEMOPLUGIN_H__
#define __DEMOPLUGIN_H__

#include "plugin.h"
#include "demoplugin2.h"

class demoPluginMessage : public PluginMessage {
    public:
        uint8_t demoPluginVar1 = 23;
};

class demoPlugin : public Plugin
{
public:
    demoPlugin() : Plugin(999,"demo") {}
    demoPlugin(int id, const char* name) : Plugin(id,name) {}
    void setup() {
        addTimerCb(SECOND,[](){
            DPRINTLN(DBG_INFO, F("demoplugin.tick"));
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
      }

     void internalCallback(PluginMessage *message) {
         DPRINTLN(DBG_INFO, F("demoplugin.internalCallback "));
         demoPlugin2Message* mes = static_cast<demoPlugin2Message*>(message);
         if(mes) {
            DPRINTLN(DBG_INFO, F("demoplugin message received: ")+String(mes->demoPlugin2Var1));
         }
     }
    MqttMessage mqttMsg;
};

#endif /*__DEMOPLUGIN_H__*/