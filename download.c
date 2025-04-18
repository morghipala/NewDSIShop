#include <stdlib.h>
#include <stdio.h>
#include "http.h"
#include "download.h"
#include <fat.h>

int download_file(const char *url, const char *dest_path) {
    char *data;
    int len;
    if (http_get(url, &data, &len) < 0) return -1;

    FILE *f = fopen(dest_path, "wb");
    if (!f) {
        free(data);
        return -2;
    }
    fwrite(data, 1, len, f);
    fclose(f);
    free(data);
    return 0;
}
