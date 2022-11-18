#ifndef __PLUGINMANAGER_H__
#define __PLUGINMANAGER_H__

#include "thirdpartyapp.h"
#include "demoplugin.h"

class pluginManager : public thirdpartyApp
{
public:
     void setup(app *app) {
        for(int i = 0; i < plugincount; i++) {
            plugins[i]->setup(app);
        }
    }
     void loop(app *app) {
        for(int i = 0; i < plugincount; i++) {
            plugins[i]->loop(app);
        }
     }
     void inverterCallback(uint8_t inverterId, uint8_t fieldId, float value) {
        for(int i = 0; i < plugincount; i++) {
            plugins[i]->inverterCallback(inverterId,fieldId,value);
        }
     }
     void mqttCallback(char *topic, byte *payload, unsigned int length) {
        for(int i = 0; i < plugincount; i++) {
            plugins[i]->mqttCallback(topic,payload,length);
        }
     }
    void sendMqtt(mqtt *mqtt) {
        for(int i = 0; i < plugincount; i++) {
            plugins[i]->sendMqtt(mqtt);
        }
     }
     private:
     // instantiate plugins here
    demoPlugin plugin1;
     // Plugin plugin2;

    // no idea how to do it the 'c++ way'
    // java would be more fun :)
    static const int plugincount = 1;
    thirdpartyApp* plugins[plugincount] = {&plugin1};


};

#endif /*__PLUGINMANAGER_H__*/