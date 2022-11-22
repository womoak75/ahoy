//-----------------------------------------------------------------------------
// 2022 Ahoy, https://www.mikrocontroller.net/topic/525778
// Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//-----------------------------------------------------------------------------

#include "utils/dbg.h"
#include "app.h"
#include "thirdparty/pluginmanager.h"
#include "config.h"


app myApp;
#ifdef THIRDPARTY
pluginManager pluginmanager;
#endif

//-----------------------------------------------------------------------------
IRAM_ATTR void handleIntr(void) {
    myApp.handleIntr();
}


//-----------------------------------------------------------------------------
void setup() {
#ifdef THIRDPARTY
    myApp.setThirdpartyApp(&pluginmanager);
#endif
    myApp.setup(WIFI_TRY_CONNECT_TIME);

    // TODO: move to HmRadio
    attachInterrupt(digitalPinToInterrupt(myApp.getIrqPin()), handleIntr, FALLING);
}


//-----------------------------------------------------------------------------
void loop() {
    myApp.loop();
}
