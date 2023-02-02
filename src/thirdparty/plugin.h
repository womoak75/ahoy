#ifndef __PLUGIN_H__
#define __PLUGIN_H__

class PluginMessage
{
public:
    const float truefloat = 1.0;
    const float falsefloat = 0.0;
    int pluginid = 0;
    const char *valuename = NULL;
    float value = 0;
};

class InverterMessage
{
public:
     /**
     * invererId - id of inverter ( 0 - (MAX_NUM_INVERTERS-1) )
     */
    uint8_t inverterId;
    /**
     *  fieldId - see hmDefines.h => field types
     */
    uint8_t fieldId;
    /**
    *value - value transmited by inverter
    */
    float value;
};

class MqttMessage
{
public:
    const char *topic;
    uint8_t *payload;
    unsigned int length;
    bool appendTopic = true;
};

typedef enum
{
    SECOND,
    MINUTE
} PLUGIN_TIMER_INTVAL;
class Plugin;

class System
{
public:
    virtual void subscribeMqtt(Plugin *plugin, char *topic, bool append) = 0;
    virtual void ctrlRequest(Plugin *plugin, JsonObject request) = 0;
    virtual bool enqueueMessage(Plugin *sender, char *topic, char *data, bool append) = 0;
    virtual void publishInternal(Plugin *sender, PluginMessage *message) = 0;
    virtual void addTimerCb(Plugin *plugin, PLUGIN_TIMER_INTVAL intval, uint32_t interval, std::function<void(void)> timerCb) = 0;
    virtual const Plugin *getPluginById(int pluginid);
    virtual const Plugin *getPluginByName(const char *pluginname);
    virtual int getPluginCount();
};

/**
 * Plugin interface
 */
class Plugin
{
public:
    Plugin(int _id, const char *_name)
    {
        id = _id;
        name = _name;
    }
    int getId() { return id; }
    void setSystem(System *s)
    {
        system = s;
    }
    /**
     * setup
     *
     * called at end of ahoi main setup
     *
     * @param app - pointer to ahoi settings
     */
    virtual void setup() = 0;
    /**
     * loop
     *
     * called at end of ahoi main loop
     *
     * @param app - pointer to ahoi app
     */
    virtual void loop() = 0;
    /**
     * inverterCallback
     *
     * will be called at end of Inverter.addValue(...)
     *
     *  @param InverterMessage
     */
    virtual void inverterCallback(const InverterMessage *message) {}
    /**
     * mqttCallback
     *
     * will be called at end of app::cbMqtt
     *
     *  @param MqttMessage
     */
    virtual void mqttCallback(const MqttMessage *message) {}
    /**
     * internalCallback
     *
     * will be called from 'system'
     *
     *  @param PluginMessage
     */
    virtual void internalCallback(const PluginMessage *message) {}
    /**
     * called when json message was posted to /thirdpartyplugins.
     * message must contain either 'pluginid' or 'pluginname'.
     * 
     * e.g.
     * 
     * {"pluginname":"someplugin","paramname":"paramvalue"}
     * 
     *  @param request - JsonObject
     *  @param respone - JsonObject
     *  @return true if request was handled - false otherwise
     */
    virtual bool onRequest(JsonObject request, JsonObject response) { return false; }
    /**
     * called when mqtt was connected/reconnected 
     * subscribe your topics here! :)
     */
    virtual void onMqttSubscribe() {}
    /**
     * @brief called when settings are loaded
     * 
     * @param settings - jsonobject with plugin config
     */
    virtual void loadSettings(JsonObject settings) {}
    /**
     * called when settings are saved
     * 
     * @param settings - jsonobject for plugin config
     */
    virtual void saveSettings(JsonObject settings) {}
    /**
     * subscribe mqtt topic. 
     * use: onMqttSubscribe()
     * 
     * @param topic
     * @param append - true if topic should be appended to DEFAULT TOPIC PREFIX - false otherwise
     */
    void subscribeMqtt(char *topic, bool append)
    {
        if (system)
        {
            system->subscribeMqtt(this, topic, append);
        }
    }
    /**
     * @brief send control request 
     * 
     * @param request - json request see RestApi.h setCtrl() for content details
     */
    void sendCtrlRequest(JsonObject request)
    {
        if (system)
        {
            system->ctrlRequest(this, request);
        }
    }
    /**
     * @brief enqueue mqtt message
     * 
     * @param topic - mqtt topic
     * @param data - data to be send
     * @param append - true: {DEFAULT MQTT TOPIC}/{pluginname}/{topic}, false: {topic}
     * @return true if message was queued successful - false otherwise 
     */
    bool enqueueMessage(char *topic, char *data, bool append = true)
    {
        if (system)
        {
            return system->enqueueMessage(this, topic, data, append);
        }
        return false;
    }
    /**
     * @brief publish internal message to all plugins
     * 
     * @param valueid - value identifier
     * @param value
     */
    void publishInternalValue(const char *valueid, float value)
    {
        if (system)
        {
            PluginMessage message;
            message.pluginid = this->getId();
            message.valuename = valueid;
            message.value = value;
            system->publishInternal(this, &message);
        }
    }
    /**
     * @brief add timer callback.
     * 
     * @param intvaltype - MINUTE / SECOND
     * @param interval
     * @param timerCb - callback function
     */
    void addTimerCb(PLUGIN_TIMER_INTVAL intvaltype, uint32_t interval, std::function<void(void)> timerCb)
    {
        if (system)
        {
            system->addTimerCb(this, intvaltype, interval, timerCb);
        }
    }
    const char *name;

private:
    int id;
    System *system;
};
#endif