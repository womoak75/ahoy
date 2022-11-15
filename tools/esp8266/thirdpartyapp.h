//-----------------------------------------------------------------------------
// 2022 Ahoy, https://www.mikrocontroller.net/topic/525778
// Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//-----------------------------------------------------------------------------

#ifndef __THIRDPARTYAPP_H__
#define __THIRDPARTYAPP_H__

#include <cstdint>
#include "app.h"

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
};

#endif /*__THIRDPARTYAPP_H__*/