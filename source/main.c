#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wifi.h"
#include "http.h"
#include "json.h"
#include "download.h"
#include <fat.h>

#define LIST_URL "https://raw.githubusercontent.com/morghipala/NewDSIShop/refs/heads/main/example.json"

int main(void) {
    // init schermi e console
    videoSetMode(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    consoleDemoInit();

    irqEnable(IRQ_VBLANK);
    fatInitDefault();
    wifi_init();

    // 1) fetch lista
    consoleClear();
    printf("Fetching game list...\n");
    char *listData; int listLen;
    if (http_get(LIST_URL, &listData, &listLen) < 0) {
        printf("Errore HTTP\n"); 
        while(1) swiWaitForVBlank();
    }

    int gameCount;
    Game *games = parse_game_list(listData, &gameCount);
    free(listData);
    if (!games) {
        printf("Parse error\n");
        while(1) swiWaitForVBlank();
    }

    int sel = 0;
    while(1) {
        consoleClear();
        printf("Select a game:\n\n");
        for (int i = 0; i < gameCount; i++) {
            if (i == sel) printf(" > %s\n", games[i].title);
            else          printf("   %s\n", games[i].title);
        }

        swiWaitForVBlank();
        scanKeys();
        int keys = keysDown();
        if (keys & KEY_DOWN) sel = (sel + 1) % gameCount;
        if (keys & KEY_UP)   sel = (sel - 1 + gameCount) % gameCount;
        if (keys & KEY_A) {
            // 2) fetch dettagli
            consoleClear();
            printf("Loading details...\n");
            char detailsUrl[256];
            sprintf(detailsUrl,
                "https://raw.githubusercontent.com/morghipala/NewDSIShop/refs/heads/main/%s.json",
                games[sel].id);

            char *detData; int detLen;
            http_get(detailsUrl, &detData, &detLen);
            Game det = parse_game_details(detData);
            free(detData);

            // 3) UI dettagli & install
            while(1) {
                consoleClear();
                printf("%s\n\n%s\n\n[A] Install  [B] Back",
                       det.title, det.description);
                swiWaitForVBlank();
                scanKeys();
                int k = keysDown();
                if (k & KEY_B) break;
                if (k & KEY_A) {
                    consoleClear();
                    printf("Downloading...\n");
                    char dest[128];
                    sprintf(dest, "/%s.nds", det.id);
                    int res = download_file(det.file_url, dest);
                    if (res == 0)      printf("Saved: %s\n", dest);
                    else               printf("Error %d\n", res);
                    printf("\nPress B");
                    while(!(keysDown() & KEY_B)) swiWaitForVBlank();
                    break;
                }
            }

            // libera dettagli
            free(det.id);
            free(det.title);
            free(det.description);
            free(det.file_url);
        }
    }

    // pulizia (mai raggiunta)
    free_game_list(games, gameCount);
    return 0;
}
