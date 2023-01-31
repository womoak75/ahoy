#ifndef __PLUGINAPP_H__
#define __PLUGINAPP_H__

#include "app.h"
#include "plugin.h"
#include "web/html/h/thirdparty_html.h"

class pluginapp : public app, public System, public settingsCb, public mqttCb, public restCb, public inverterCb
{
public:
    pluginapp() : app() {}
    ~pluginapp() {}
    void setupSettings(settings* appSettings) {
        mSettings = appSettings;
        mSettings->mSettingsCb = this;
    }
    void setupCB(PubMqttType *mqtt, WebType *webtype, RestApiType *restapi)
    {
        DPRINTLN(DBG_INFO, F("setupCB: "));
        mMqtt = mqtt;
        mRestapi = restapi;
        mqtt->mMqttCb = this;
        webtype->getWebSrvPtr()->on("/thirdparty", HTTP_ANY, std::bind(&pluginapp::onHttp, this, std::placeholders::_1));
        webtype->getWebSrvPtr()->on("/thirdpartysetup", HTTP_GET,std::bind(&pluginapp::onThirdpartySetup, this, std::placeholders::_1));
        webtype->getWebSrvPtr()->addHandler(new AsyncCallbackJsonWebHandler("/thirdpartysave", [this](AsyncWebServerRequest *request, JsonVariant &json) {
            JsonObject jsonObj = json.as<JsonObject>();
            loadThirdpartySettings(jsonObj[F("thirdparty")]);
            triggerSave();
            // :)
            request->send(200,F("application/json"),F("{}"));
        }));
        restapi->mRestCb = this;
        Inverter<> *iv;
        for (uint8_t i = 0; i < mSys->getNumInverters(); i++)
        {
            iv = mSys->getInverterByPos(i, false);
            if (NULL != iv)
            {
                iv->mInverterCb = this;
            }
        }
    }
    void setup()
    {
        app::setup();
        for (unsigned int i = 0; i < plugins.size(); i++)
        {
            plugins[i]->setSystem(this);
            plugins[i]->setup();
            if (strlen(plugins[i]->name) > maxnamelen)
            {
                maxnamelen = strlen(plugins[i]->name);
            }
        }
    }
    void loop()
    {
        app::loop();
        for (unsigned int i = 0; i < plugins.size(); i++)
        {
            plugins[i]->loop();
        }
        publish();
        if(saveTpSettings) {
            saveTpSettings = false;
            mSettings->saveSettings();
        }
    }

    void onThirdpartySetup(AsyncWebServerRequest *request) {
            AsyncJsonResponse* response = new AsyncJsonResponse(false, 4096);
            JsonObject root = response->getRoot();
            onSaveSettings(root);
            response->setLength();
            request->send(response);
    }

    void onInverterValue(uint8_t inverterId, uint8_t fieldId, float value)
    {
        DPRINTLN(DBG_INFO, F("onInverterValue"));
        InverterMessage message;
        message.inverterId = inverterId;
        message.fieldId = fieldId;
        message.value = value;
        for (unsigned int i = 0; i < plugins.size(); i++)
        {
            plugins[i]->inverterCallback(&message);
        }
    }
    void ctrlRequest(Plugin *plugin, JsonObject request)
    {
        mRestapi->ctrlRequest(request);
    }
    /**
     * enqueue a mqtt message in send queue
     * @param topic - mqtt topic
     * @param message - mqtt payload
     * @param appendTopic - append topic to ahoi prefix (inverter/)
     * @return true, if message was enqueued, false otherwise
     */
    bool enqueueMessage(Plugin *plugin, char *topic, char *data, bool append)
    {
        size_t topiclen = strlen(topic) + 1;
        size_t datalen = strlen(data) + 1;
        if (bufferindex + topiclen + datalen > THIRDPARTY_MSG_BUFFERSIZE)
        {
            return false;
        }
        qentry entry;
        entry.topicindex = bufferindex;
        memcpy(buffer + bufferindex, topic, topiclen);
        bufferindex += topiclen;
        entry.dataindex = bufferindex;
        memcpy(buffer + bufferindex, data, datalen);
        bufferindex += datalen;
        entry.appendtopic = append;
        entry.pluginid = plugin->getId();
        q.push(entry);
        return true;
    }
    void subscribeMqtt(Plugin *plugin, char *topic, bool append)
    {
        if (append)
        {
            mMqtt->subscribe(topic);
        }
        else
        {
            mMqtt->subscribeThirdparty(topic);
        }
    }
    void publish()
    {
        // we dont care about real topic length, one size fit's all ;)
        char topic[128];
        while (!q.empty() && mMqtt->isConnected())
        {
            qentry entry = q.front();
            auto sender = getPluginById(entry.pluginid);
            if (NULL != sender)
            {
                snprintf(topic, sizeof(topic), "%s/%s", sender->name, (const char *)buffer + entry.topicindex);
                mMqtt->publish(topic, (const char *)buffer + entry.dataindex, false, entry.appendtopic);
            }
            q.pop();
        }

        bufferindex = 0;
    }

    void addTimerCb(Plugin *plugin, PLUGIN_TIMER_INTVAL intvaltype, uint32_t interval, std::function<void(void)> timerCb)
    {
        if (intvaltype == PLUGIN_TIMER_INTVAL::MINUTE)
        {
            every(timerCb, (interval * 60));
        }
        else if (intvaltype == PLUGIN_TIMER_INTVAL::SECOND)
        {
            every(timerCb, interval);
        }
    }

    void publishInternal(Plugin *plugin, PluginMessage *message)
    {
        message->pluginid = plugin->getId();
        for (unsigned int i = 0; i < plugins.size(); i++)
        {
            if (plugins[i]->getId() != plugin->getId())
            {
                plugins[i]->internalCallback(message);
            }
        }
    }
    virtual const Plugin *getPluginById(int pluginid)
    {
        for (unsigned int i = 0; i < plugins.size(); i++)
        {
            if (plugins[i]->getId() == pluginid)
            {
                return plugins[i];
            }
        }
        return NULL;
    }

    virtual const Plugin *getPluginByName(const char *pluginname)
    {
        for (unsigned int i = 0; i < plugins.size(); i++)
        {
            if (strcmp(plugins[i]->name, pluginname) == 0)
            {
                return plugins[i];
            }
        }
        return NULL;
    }
    virtual int getPluginCount()
    {
        return plugins.size();
    }
    void addPlugin(Plugin *p)
    {
        plugins.push_back(p);
    }

    void onLoadSettings(DynamicJsonDocument settings)
    {
        DPRINTLN(DBG_INFO, F("onSettingsAction: load settings"));
        if (settings.containsKey(F("thirdparty")))
        {
            loadThirdpartySettings(settings[F("thirdparty")]);
        }
    }

    void loadThirdpartySettings(JsonObject tpsettings) {
            DPRINTLN(DBG_INFO, F("loadThirdpartySettings"));
            for (unsigned int i = 0; i < plugins.size(); i++)
            {
                if (tpsettings.containsKey(plugins[i]->name))
                {
                    plugins[i]->loadSettings(tpsettings[plugins[i]->name]);
                }
            }
    }

    void triggerSave() {
        saveTpSettings = true;
    }

    void onGetSetup(JsonObject settings) {
        onSaveSettings(settings);
    }

    void onSaveSettings(JsonObject settings)
    {
        DPRINTLN(DBG_INFO, F("onSettingsAction: save settings"));
        JsonObject tpsettings = settings.createNestedObject(F("thirdparty"));
        for (unsigned int i = 0; i < plugins.size(); i++)
        {
            JsonObject pluginjson = tpsettings.createNestedObject(plugins[i]->name);
            //pluginjson[F("id")] = plugins[i]->getId();
            plugins[i]->saveSettings(pluginjson);
        }
    }

    void onMqttConnect()
    {
        mMqtt->subscribe("thirdparty/#");
        for (unsigned int i = 0; i < plugins.size(); i++)
        {
            plugins[i]->onMqttSubscribe();
        }
    }
    void onMqttMessage(const char *topic, const uint8_t *payload, size_t len)
    {
        DPRINTLN(DBG_INFO, F("onMessage: ") + String(topic));
        MqttMessage msg;
        msg.topic = (char *)topic;
        msg.payload = (uint8_t *)payload;
        msg.length = len;
        for (unsigned int i = 0; i < plugins.size(); i++)
        {
            plugins[i]->mqttCallback(&msg);
        }
    }
    void onRestMenu(JsonObject obj, uint8_t index)
    {
        DPRINTLN(DBG_INFO, F("onMenu"));
        obj[F("name")][index] = "Thirdparty";
        obj[F("link")][index++] = "/thirdparty";
    }

private:
    void onHttp(AsyncWebServerRequest *request)
    {
        DPRINTLN(DBG_INFO, F("onHttp"));
        AsyncWebServerResponse *response = request->beginResponse_P(200, F("text/html"), thirdparty_html, thirdparty_html_len);
        response->addHeader(F("Content-Encoding"), "gzip");
        request->send(response);
    }

    PubMqttType *mMqtt;
    RestApiType *mRestapi;
    settings *mSettings;
    bool saveTpSettings = false;
    char buffer[THIRDPARTY_MSG_BUFFERSIZE];
    uint16_t bufferindex = 0;
    typedef struct
    {
        int pluginid;
        uint16_t topicindex;
        uint16_t dataindex;
        bool appendtopic;
    } qentry;
    std::queue<qentry> q;
    unsigned int maxnamelen = 0;
    std::vector<Plugin *> plugins;
};

#endif /*__PLUGINAPP_H__*/
