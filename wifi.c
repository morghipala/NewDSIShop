#include <nds.h>
#include <nds/ndstypes.h>
#include <nds/wifi.h>
#include <stdio.h>
#include "wifi.h"

void wifi_init(void) {
    consoleSelect(0);
    printf("Init WiFi...\n");
    irqEnable(IRQ_VBLANK);
    wifiInitDefault();
    while(wifiGetStatus() != IPS_DONE) {
        swiWaitForVBlank();
    }
    printf("WiFi ready.\n");
    swiWaitForVBlank();
}
