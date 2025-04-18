#include <nds.h>
#include <dswifi9.h>
#include <string.h>
#include <stdlib.h>
#include "http.h"

// parsing base di URL "https://host/path" o "http://host/path"
static void parse_url(const char *url, char *host, char *path) {
    const char *p = strstr(url, "://");
    if (!p) p = url;
    else       p += 3;
    const char *slash = strchr(p, '/');
    if (slash) {
        int hlen = slash - p;
        strncpy(host, p, hlen);
        host[hlen] = 0;
        strcpy(path, slash);
    } else {
        strcpy(host, p);
        strcpy(path, "/");
    }
}

int http_get(const char *url, char **outData, int *outLen) {
    char host[128], path[256];
    parse_url(url, host, path);

    int sock = wifi_tcp_connect(host, 80);
    if (sock < 0) return -1;

    char req[512];
    sprintf(req,
        "GET %s HTTP/1.0\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n\r\n",
        path, host);

    wifi_tcp_send(sock, req, strlen(req));

    int capacity = 1024;
    char *buffer = malloc(capacity);
    int total = 0;
    while (1) {
        int r = wifi_tcp_recv(sock, buffer + total, capacity - total);
        if (r <= 0) break;
        total += r;
        if (capacity - total < 256) {
            capacity *= 2;
            buffer = realloc(buffer, capacity);
        }
    }

    // separa header e body
    char *body = strstr(buffer, "\r\n\r\n");
    if (!body) {
        free(buffer);
        *outData = NULL;
        *outLen = 0;
        wifi_tcp_close(sock);
        return 0;
    }

    body += 4; // salta "\r\n\r\n"
    int bodyLen = total - (body - buffer);

    char *data = malloc(bodyLen + 1);
    memcpy(data, body, bodyLen);
    data[bodyLen] = '\0';

    *outData = data;
    *outLen = bodyLen;

    free(buffer);
    wifi_tcp_close(sock);
    return 0;
}
