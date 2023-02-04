//-----------------------------------------------------------------------------
// 2022 Ahoy, https://www.mikrocontroller.net/topic/525778
// Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//-----------------------------------------------------------------------------

#include "utils/dbg.h"
#include "app.h"
#include "config/config.h"
#ifdef THIRDPARTY
#include "thirdparty/pluginapp.h"
#include "thirdparty/demoplugin.h"
#include "thirdparty/demoplugin2.h"
#endif

#ifdef THIRDPARTY
pluginapp myApp;
demoPlugin plugin1 = demoPlugin();
demoPlugin2 plugin2 = demoPlugin2();
#else
app myApp;
#endif

//-----------------------------------------------------------------------------
IRAM_ATTR void handleIntr(void) {
    myApp.handleIntr();
}


//-----------------------------------------------------------------------------
void setup() {
    #ifdef THIRDPARTY
    myApp.addPlugin(&plugin1);
    myApp.addPlugin(&plugin2);
    #endif
    myApp.setup();


    // TODO: move to HmRadio
    attachInterrupt(digitalPinToInterrupt(myApp.getIrqPin()), handleIntr, FALLING);
}


//-----------------------------------------------------------------------------
void loop() {
    myApp.loop();
}
