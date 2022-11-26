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
#include <array>

#ifndef THIRDPARTY_PLUGIN_COUNT
#define THIRDPARTY_PLUGIN_COUNT 1
#endif

class pluginManager : public thirdpartyApp
{
public:
    pluginManager() : thirdpartyApp(0,"pluginmanager") {}
    
    void setup(app *app, settings_t *settings) {
        thirdpartyApp::setup(app,settings);
        for(int i = 0; i < plugincount; i++) {
            plugins[i]->setSystem(this);
            plugins[i]->setup();
            if(strlen(plugins[i]->name) > maxnamelen) {
                maxnamelen = strlen(plugins[i]->name);
            }
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
    void internalCallback(char *topic, byte *payload, unsigned int length) {
        for(int i = 0; i < plugincount; i++) {
            plugins[i]->internalCallback(topic,payload,length);
        }
     }
    void publishInternal(Plugin *plugin, char *dataid, byte *payload, unsigned int length) {
        char topic[strlen(dataid)+maxnamelen+2];
        // pretty 'hacky' :/
        // topic structure '{pluginname}/{dataidentifier}'
        snprintf(topic,sizeof(topic),"%s/%s",plugin->name,dataid);
        for(int i = 0; i < plugincount; i++) {
            if(plugins[i]->getId() != plugin->getId()) {
                plugins[i]->internalCallback(topic,payload,length);
            }
        }
    }
    virtual const Plugin* getPluginById(int pluginid) {
        for(int i = 0; i < plugincount; i++) {
            if(plugins[i]->getId() == pluginid) {
                return plugins[i];
            }
        }
        return this; // :)
    }
    virtual const Plugin* getPluginByName(const char* pluginname) {
        for(int i = 0; i < plugincount; i++) {
            if(strcmp(plugins[i]->name,pluginname)==0) {
                return plugins[i];
            }
        }
        return this; // :)
    }
    virtual int getPluginCount() {
        return plugincount;
    }
    private:
        int maxnamelen = 0;
        int plugincount = THIRDPARTY_PLUGIN_COUNT;

        demoPlugin plugin1 = demoPlugin(1,"demoplugin");;
        #ifdef METERPLUGIN
        meterPlugin plugin2 = meterPlugin(2);
        #endif
        #ifdef POWERCONTROLLERPLUGIN
        powerControllerPlugin plugin3 = powerControllerPlugin(3);
        #endif
        Plugin* plugins[THIRDPARTY_PLUGIN_COUNT] = {
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