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
    uint8_t inverterId;
    uint8_t fieldId;
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
    virtual void subscribeMqtt(Plugin *plugin, char* topic, bool append) = 0;
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
     *  @param invererId - id of inverter ( 0 - (MAX_NUM_INVERTERS-1) )
     *  @param fieldId - see hmDefines.h => field types
     *  @param value - value transmited by inverter
     */
    virtual void inverterCallback(const InverterMessage *message) {}
    /**
     * mqttCallback
     *
     * will be called at end of app::cbMqtt
     *
     *  @param topic - topic for which data was receiced (e.g. 'inverter/thirdparty/cmd')
     *  @param payload - byte* for received data
     *  @param length - length of payload
     */
    virtual void mqttCallback(const MqttMessage *message) {}
    /**
     * internalCallback
     *
     * will be called from 'system'
     *
     *  @param topic - topic for which data was receiced (e.g. 'powercontroller/power/limit')
     *  @param payload - byte* for received data
     *  @param length - length of payload
     */
    virtual void internalCallback(const PluginMessage *message) {}

    virtual void onMqttSubscribe() {}

    void subscribeMqtt(char *topic, bool append) {
        if (system)
        {
            system->subscribeMqtt(this, topic, append);
        }
    }

    bool enqueueMessage(char *topic, char *data, bool append)
    {
        if (system)
        {
            return system->enqueueMessage(this, topic, data, append);
        }
        return false;
    }

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

    void addTimerCb(PLUGIN_TIMER_INTVAL intvaltype, uint32_t interval, std::function<void(void)> timerCb)
    {
        if (system)
        {
            system->addTimerCb(this, intvaltype, interval, timerCb);
        }
    }
    const char *name;

protected:
    System *getSystem() { return system; }

private:
    int id;
    System *system;
};
#endif