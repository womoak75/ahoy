#ifndef __DEMOPLUGIN_H__
#define __DEMOPLUGIN_H__

#include "thirdpartyapp.h"

class demoPlugin : public Plugin
{
public:
    demoPlugin() : Plugin(999,"demo") {}
    demoPlugin(int id, const char* name) : Plugin(id,name) {}
    void setup() {
        
    }
    void loop() {
        // main loop
        char topic[] = "demoplugin/out";
        char payload[] = "ahoi world!";
        static unsigned long last = 0;
        if((millis()-last)>=55000) {
          last = millis();
          // send mqtt message to 'DEF_MQTT_TOPIC/demoplugin/out'
          // default for DEF_MQTT_TOPIC = "inverter" (see config.h)
          getSystem()->enqueueMessage(this,topic,payload,true);
          // send mqtt message to 'demoplugin/out'
          getSystem()->enqueueMessage(this,topic,payload,false);
        }
     }
     void inverterCallback(uint8_t inverterId, uint8_t fieldId, float value) {
        // receice inverter data
     }
     void mqttCallback(char *topic, byte *payload, unsigned int length) {
         // receive data for
         // ahoi topic: 'DEF_MQTT_TOPIC/devcontrol/#'
         // thirdparty topic: 'DEF_MQTT_TOPIC/thirdparty/#'
         // default for DEF_MQTT_TOPIC = "inverter" (see config.h)
      }

     void internalCallback(char *topic, byte *payload, unsigned int length) {
         // internal topic: '{pluginname}/{dataidentifier}'
         
     }
};

#endif /*__DEMOPLUGIN_H__*/