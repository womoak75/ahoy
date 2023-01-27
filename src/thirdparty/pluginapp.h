#ifndef __PLUGINAPP_H__
#define __PLUGINAPP_H__

#include "app.h"
#include "plugin.h"
#include "web/html/h/thirdparty_html.h"

class pluginapp : public app, public System
{
public:
    pluginapp() : app() {}
    ~pluginapp() {}
    void setupCB(PubMqttType *mqtt, WebType *webtype, RestApiType *restapi)
    {
        DPRINTLN(DBG_INFO, F("setupCB: "));
        mMqtt = mqtt;
        mqtt->setMessageCb(std::bind(&pluginapp::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        webtype->getWebSrvPtr()->on("/thirdparty", HTTP_ANY, std::bind(&pluginapp::onHttp, this, std::placeholders::_1));
        restapi->mJsonCb = std::bind(&pluginapp::onMenu, this, std::placeholders::_1, std::placeholders::_2);
        Inverter<> *iv;
        for (uint8_t i = 0; i < mSys->getNumInverters(); i++)
        {
            iv = mSys->getInverterByPos(i, false);
            if (NULL != iv)
            {
                iv->mInverterCb = (std::bind(&pluginapp::inverterCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
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
    }
    void onMessage(const char *topic, const uint8_t *payload, size_t len)
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
    void onHttp(AsyncWebServerRequest *request)
    {
        DPRINTLN(DBG_INFO, F("onHttp"));
        AsyncWebServerResponse *response = request->beginResponse_P(200, F("text/html"), thirdparty_html, thirdparty_html_len);
        response->addHeader(F("Content-Encoding"), "gzip");
        request->send(response);
    }
    void onMenu(JsonObject obj,uint8_t index)
    {
        DPRINTLN(DBG_INFO, F("onMenu"));
        obj[F("name")][index] = "Thirdparty";
        obj[F("link")][index++] = "/thirdparty";
    }
    void inverterCallback(uint8_t inverterId, uint8_t fieldId, float value)
    {
        DPRINTLN(DBG_INFO, F("inverterCallback"));
        InverterMessage message;
        message.inverterId = inverterId;
        message.fieldId = fieldId;
        message.value = value;
        for (unsigned int i = 0; i < plugins.size(); i++)
        {
            plugins[i]->inverterCallback(&message);
        }
    }

    /**
     * enqueue a mqtt message in send queue
     * @param topic - mqtt topic
     * @param message - mqtt payload
     * @param appendTopic - append topic to ahoi prefix (inverter/)
     * @return true, if message was enqueued, false otherwise
     */
    bool enqueueMessage(Plugin *plugin, MqttMessage *msg)
    {
        size_t topiclen = strlen(msg->topic) + 1;
        size_t datalen = msg->length + 1;
        if (bufferindex + topiclen + datalen > THIRDPARTY_MSG_BUFFERSIZE)
        {
            return false;
        }
        qentry entry;
        entry.topicindex = bufferindex;
        memcpy(buffer + bufferindex, msg->topic, topiclen);
        bufferindex += topiclen;
        entry.dataindex = bufferindex;
        memcpy(buffer + bufferindex, msg->payload, datalen);
        bufferindex += datalen;
        entry.appendtopic = msg->appendTopic;
        entry.pluginid = plugin->getId();
        q.push(entry);
        return true;
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

    void addTimerCb(Plugin *plugin, PLUGIN_TIMER_INTVAL intval, std::function<void(void)> timerCb)
    {
        if (intval == PLUGIN_TIMER_INTVAL::MINUTE)
        {
            everyMin(timerCb);
        }
        else if (intval == PLUGIN_TIMER_INTVAL::SECOND)
        {
            everySec(timerCb);
        }
    }

    void publishInternal(Plugin *plugin, PluginMessage *message)
    {
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

private:
    PubMqttType *mMqtt;
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
