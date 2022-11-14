#ifndef __THIRDPARTYAPP_H__
#define __THIRDPARTYAPP_H__

#include <cstdint>
#include "app.h"

class thirdpartyApp {
   public:
      virtual void setup(app *app) = 0;
      virtual void loop(app *app) = 0;
      virtual void inverterCallback(uint8_t inverterId, uint8_t fieldId, float value) = 0;
      virtual void mqttCallback(char *topic, byte *payload, unsigned int length) = 0;
};

#endif /*__THIRDPARTYAPP_H__*/