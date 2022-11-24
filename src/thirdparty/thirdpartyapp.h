//-----------------------------------------------------------------------------
// 2022 Ahoy, https://www.mikrocontroller.net/topic/525778
// Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//-----------------------------------------------------------------------------

#ifndef __THIRDPARTYAPP_H__
#define __THIRDPARTYAPP_H__

#ifndef THIRDPARTY_MSG_BUFFERSIZE
#define THIRDPARTY_MSG_BUFFERSIZE 256
#endif

#include <cstdint>
#include "app.h"
#include "web/html/h/thirdparty_html.h"

typedef enum {SECOND,MINUTE} PLUGIN_TIMER_INTVAL;
class Plugin;

/**
 * facade for main app (ahoi) 
 */
class System {
    public:
        virtual void setup(app *app, settings_t *settings);
        virtual bool enqueueMessage(Plugin *plugin, char *topic, char *data, bool appendTopic = true) = 0;
        virtual void publishInternal(Plugin *plugin, char *dataid, byte *payload, unsigned int length) = 0;
        virtual void addTimerCb(Plugin *plugin, PLUGIN_TIMER_INTVAL intval, std::function<void(void)> timerCb) = 0;
};

/**
 * Plugin interface
 */
class Plugin {
    public:
        Plugin(int _id) {
            id = _id;
        }
        int getId() { return id; }
        void setSystem(System *s) {
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
        virtual void inverterCallback(uint8_t inverterId, uint8_t fieldId, float value) = 0;
        /**
         * mqttCallback
         * 
         * will be called at end of app::cbMqtt
         * 
         *  @param topic - topic for which data was receiced (e.g. 'inverter/thirdparty/cmd')
         *  @param payload - byte* for received data 
         *  @param length - length of payload
         */
        virtual void mqttCallback(char *topic, byte *payload, unsigned int length) = 0;

    protected:
        System* getSystem() { return system; }

    private:
        int id;
        System *system;
};

/**
 * thirdpartyApp
 *
 * Simple interface to add custom code / apps / plugins
 * which will be executed within ahoi app context.
 *
 */
class thirdpartyApp : public Plugin , public System {
    public:
        thirdpartyApp(int id) : Plugin(id) {
            setSystem(this); // :)))
        }
        void setup() {

        }
        void setup(app *app, settings_t *settings) {
            _app = app,
            appsettings = settings;
        }
        /**
         * call when menu entry in webpage menu is clicked
         * @param request - AsyncWebServerRequest
         */
        virtual void onWebRequest(AsyncWebServerRequest *request) {
            // a custom tp app should return its own html page
            AsyncWebServerResponse *response = request->beginResponse_P(200, F("text/html"), thirdparty_html, thirdparty_html_len);
            response->addHeader(F("Content-Encoding"), "gzip");
            request->send(response);
        }
        /**
         * called when /api/thirdparty is requested via HttpGet
         * @param request - AsyncWebServerRequest
         * @param json response object - JsonObject
         */
        virtual void onApiGet(AsyncWebServerRequest *request, JsonObject root) {
            JsonObject tp = root.createNestedObject("thirdparty");
            tp["name"] = "mythirdpartyapp";
        }
        /**
         * called when data is sent to /api/thirdparty HttpPost
         * @param json request - DynamicJsonDocument
         * @param json respone object - JsonObject
         */
        virtual bool onApiPost(DynamicJsonDocument jsonIn, JsonObject jsonOut) {
            // we don't care, but pretend everything is alright - developer default ;)
            return true;
        }
        /**
         * called from ahoi main loop
         */
        void publish() {
            char topic[128];
            while(!q.empty()) {
                qentry entry = q.front();
                if(!entry.appendtopic) {
                    snprintf(topic,sizeof(topic),"%s",(const char *)buffer+entry.topicindex);
                } else {
                    snprintf(topic,sizeof(topic),"%s/%s",appsettings->mqtt.topic,(const char *)buffer+entry.topicindex);
                }
                if(publishFkt2) {
                    publishFkt2(topic,(const char *)buffer+entry.dataindex,false);
                }
                q.pop();
            }

            bufferindex = 0;
        }

        /**
         * enqueue a mqtt message in send queue
         * @param topic - mqtt topic
         * @param message - mqtt payload
         * @param appendTopic - append topic to ahoi prefix (inverter/)
         * @return true, if message was enqueued, false otherwise
         */
        bool enqueueMessage(Plugin *plugin, char *topic, char *data, bool appendTopic = true) {
            size_t topiclen = strlen(topic)+1;
            size_t datalen = strlen(data)+1;
            if(bufferindex+topiclen+datalen>THIRDPARTY_MSG_BUFFERSIZE) {
                return false;
            }
            qentry entry;
            entry.topicindex = bufferindex;
            memcpy(buffer+bufferindex,topic,topiclen);
            bufferindex += topiclen;
            entry.dataindex = bufferindex;
            memcpy(buffer+bufferindex,data,datalen);
            bufferindex += datalen;
            entry.appendtopic = appendTopic;
            q.push(entry);
            return true;
        }
        void addTimerCb(Plugin *plugin, PLUGIN_TIMER_INTVAL intval, std::function<void(void)> timerCb) {
            if(_app) {
                reinterpret_cast<ah::Scheduler*>(_app)->addListener((intval==PLUGIN_TIMER_INTVAL::MINUTE)?EVERY_MIN:EVERY_SEC,timerCb);
            }
        }
        void setPublishFkt(std::function<void(const char *topic, const char *msg, boolean retained)> _publishFkt2) {
            publishFkt2 = _publishFkt2;
        }
    protected:
            app *_app = nullptr;
            settings_t *appsettings = nullptr;
    public:
        char buffer[THIRDPARTY_MSG_BUFFERSIZE];
        uint16_t bufferindex = 0;
        typedef struct {
            uint16_t topicindex;
            uint16_t dataindex;
            bool appendtopic;
        } qentry;
        std::queue<qentry> q;
        std::function<void(const char *topic, const char *msg, boolean retained)> publishFkt2 = nullptr;
};

#endif /*__THIRDPARTYAPP_H__*/