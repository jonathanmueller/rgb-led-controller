#include "led.h"


REGISTER_APP(cycle) {
    /* The duration (ms) to keep each app open */
    const long CYCLE_DURATION = 5000;

    /* The iterator for the current app */
    std::map<String, App>::iterator currentAppIterator;

    /* Time that the app was last switched */
    long lastChange = 0;

    /* True if no other apps are found - disabled this app */
    bool disableApp = false;

    bool appSupportsCycles = false;
    bool appCycleMarked = false;


    bool shouldExcludeApp(const String & name) {
        return name == "cycle" || name == "noop" || name == "off" || name == "debug" || name == "strobe" || name == "ledgroups";
    }

    void setup() {
        currentAppIterator = getApps().begin();

        /* Skip the current app to avoid stack overflow */
        while (shouldExcludeApp(currentAppIterator->first) && currentAppIterator != getApps().end()) { currentAppIterator++; }

        /* If we don't have any apps, set a flag */
        if (currentAppIterator == getApps().end()) { disableApp = true; return; }

        lastChange = millis();

        /* Reset any marked end of cycles */
        appSupportsCycles = false;
        appCycleMarked = false;

        /* Call the setup() function for the first app */
        currentAppIterator->second.setup();
    }

    void loop() {
        appRespectsPrimaryColor = false;
        if (disableApp) { return; }

        /* Check if enough time has passed to switch to the next app */
        if (millis() - lastChange > CYCLE_DURATION && (!appSupportsCycles || appCycleMarked)) {
            do {
                /* Go to the next app */
                currentAppIterator++;

                /* If we're at the end, start again */
                if (currentAppIterator == getApps().end()) { currentAppIterator = getApps().begin(); }
            } while (shouldExcludeApp(currentAppIterator->first));

            /* Save the time we switched */
            lastChange = millis();

            /* Reset any marked end of cycles */
            appSupportsCycles = false;
            appCycleMarked = false;

            /* Call the setup() function for the next app */
            currentAppIterator->second.setup();
        }

        appCycleMarked = false;

        /* Call the loop() function for the current app */
        currentAppIterator->second.loop();
    }
}

void markAppCycle() {
    App_cycle::appSupportsCycles = true;
    App_cycle::appCycleMarked = true;
}