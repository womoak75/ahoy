#ifndef __DEMOPLUGIN_H__
#define __DEMOPLUGIN_H__

#include "thirdpartyapp.h"

class demoPlugin : public Plugin
{
public:
    demoPlugin(int id) : Plugin(id) {}
    void setup(app *app) {
        // setup
    }
    void loop(app *app) {
        // main loop
        char topic[] = "demoplugin/out";
        char payload[] = "ahoi world!";
        static unsigned long last = 0;
        if((millis()-last)>=55000) {
          last = millis();
          // send mqtt message to 'DEF_MQTT_TOPIC/demoplugin/out'
          // default for DEF_MQTT_TOPIC = "inverter" (see config.h)
          enqueueMessage(topic,payload);
          // send mqtt message to 'demoplugin/out'
          enqueueMessage(topic,payload,false);
          // ATTENTION:
          // mqtt messages will be processed every MQTT_INTERVAL seconds by ahoi app
          // default is 60s - (see config.h)
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
};

#endif /*__DEMOPLUGIN_H__*/