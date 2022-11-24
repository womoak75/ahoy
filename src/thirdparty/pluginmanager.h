#ifndef __PLUGINMANAGER_H__
#define __PLUGINMANAGER_H__

#include "thirdpartyapp.h"
#include "demoplugin.h"
#ifdef METERPLUGIN
#include "meterplugin.h"
#endif
#ifdef POWERCONTROLLERPLUGIN
#include "powercontrollerplugin.h"
#endif

class pluginManager : public thirdpartyApp
{
public:
    pluginManager() : thirdpartyApp(0) {}
    
    void setup(app *app, settings_t *settings) {
        thirdpartyApp::setup(app,settings);
        for(int i = 0; i < plugincount; i++) {
            plugins[i]->setSystem(this);
            plugins[i]->setup();
        }
    }
     void loop() {
        for(int i = 0; i < plugincount; i++) {
            plugins[i]->loop();
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
    void publishInternal(Plugin *plugin, char *dataid, byte *payload, unsigned int length) {
        char topic[strlen(dataid)+12];
        // pretty 'hacky' :/
        // topic structure 'plugins/{pluginid}/{dataidentifier}'
        snprintf(topic,sizeof(topic),"plugins/%d/%s",plugin->getId(),dataid);
        for(int i = 0; i < plugincount; i++) {
            if(plugins[i]->getId() != plugin->getId()) {
                plugins[i]->mqttCallback(topic,payload,length);
            }
        }
    }
     private:
     // instantiate plugins here
    demoPlugin plugin1 = demoPlugin(1);
#ifdef METERPLUGIN
    meterPlugin plugin2 = meterPlugin(2);
#endif
#ifdef POWERCONTROLLERPLUGIN
    powerControllerPlugin plugin3 = powerControllerPlugin(3);
#endif

    // no idea how to do it the 'c++ way'
    // java would be more fun :)
    static const int plugincount = 3;
    Plugin* plugins[plugincount] = {
        &plugin1
#ifdef METERPLUGIN
        ,&plugin2
#endif
#ifdef POWERCONTROLLERPLUGIN
        ,&plugin3
#endif
        };


};

#endif /*__PLUGINMANAGER_H__*/