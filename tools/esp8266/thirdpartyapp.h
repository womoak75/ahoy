//-----------------------------------------------------------------------------
// 2022 Ahoy, https://www.mikrocontroller.net/topic/525778
// Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//-----------------------------------------------------------------------------

#ifndef __THIRDPARTYAPP_H__
#define __THIRDPARTYAPP_H__

#include <cstdint>
#include "app.h"
#include "html/h/thirdparty_html.h"

/**
 * thirdpartyApp
 *
 * Simple interface to add custom code / apps / plugins
 * which will be executed within ahoi app context.
 *
 */
class thirdpartyApp {
    public:
        /**
         * setup
         * 
         * called at end of ahoi main setup
         * 
         * @param app - pointer to ahoi app
        */
        virtual void setup(app *app) = 0;
        /**
         * loop
         * 
         * called at end of ahoi main loop
         * 
         * @param app - pointer to ahoi app
        */
        virtual void loop(app *app) = 0;
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
};

#endif /*__THIRDPARTYAPP_H__*/