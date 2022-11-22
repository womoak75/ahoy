#ifndef __PLUGINMANAGER_H__
#define __PLUGINMANAGER_H__

#include "thirdpartyapp.h"
#include "demoplugin.h"

class pluginManager : public thirdpartyApp
{
public:
    pluginManager() : thirdpartyApp(0) {}
     void setup(app *app) {
        for(int i = 0; i < plugincount; i++) {
            plugins[i]->setSystem(this);
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
     private:
     // instantiate plugins here
    demoPlugin plugin1 = demoPlugin(1);
     // Plugin plugin2;

    // no idea how to do it the 'c++ way'
    // java would be more fun :)
    static const int plugincount = 1;
    Plugin* plugins[plugincount] = {&plugin1};


};

#endif /*__PLUGINMANAGER_H__*/