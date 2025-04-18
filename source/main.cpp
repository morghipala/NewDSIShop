#include <nds.h>
#include <stdio.h>

static volatile int frame = 0;
static int selectedItem = 0;
static const int NUM_ITEMS = 5;

static void Vblank() {
    frame++;
}

void drawShopInterface() {
    consoleClear();
    
    // Header
    iprintf("\x1b[32;1m=== DSi Shop ===\x1b[39m\n\n");
    
    // Items list
    const char* items[] = {
        "Super Mario Bros",
        "Pokemon Black",
        "Zelda Spirit Tracks",
        "New Super Mario Bros",
        "Brain Training"
    };
    
    for(int i = 0; i < NUM_ITEMS; i++) {
        if(i == selectedItem) {
            iprintf("\x1b[32m> %s\x1b[39m\n", items[i]);
        } else {
            iprintf("  %s\n", items[i]);
        }
    }
    
    // Footer
    iprintf("\n\x1b[33mUsa il touch per selezionare\nSTART per uscire\x1b[39m");
}

int main(void) {
    touchPosition touchXY;
    
    irqSet(IRQ_VBLANK, Vblank);
    consoleDemoInit();
    
    while(pmMainLoop()) {
        swiWaitForVBlank();
        scanKeys();
        int keys = keysDown();
        
        if (keys & KEY_START) break;
        
        touchRead(&touchXY);
        
        // Touch input handling for item selection
        if(keys & KEY_TOUCH) {
            int touchY = touchXY.py;
            selectedItem = (touchY - 16) / 8; // Adjust based on font height
            if(selectedItem < 0) selectedItem = 0;
            if(selectedItem >= NUM_ITEMS) selectedItem = NUM_ITEMS - 1;
        }
        
        drawShopInterface();
    }
    
    return 0;
}
