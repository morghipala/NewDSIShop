#include <string.h>
#include <stdlib.h>
#include "json.h"
#include "jsmn.h"

// helper per confrontare token stringa
static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    return (tok->type == JSMN_STRING &&
            (int)strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0);
}

// copia substring tokenizzata in buffer mallocato
static char *tokcpy(const char *json, jsmntok_t *tok) {
    int len = tok->end - tok->start;
    char *s = malloc(len + 1);
    memcpy(s, json + tok->start, len);
    s[len] = '\0';
    return s;
}

Game *parse_game_list(const char *json, int *outCount) {
    jsmn_parser p; jsmntok_t toks[256];
    jsmn_init(&p);
    int ntok = jsmn_parse(&p, json, strlen(json), toks, 256);
    if (ntok < 0) return NULL;
    if (toks[0].type != JSMN_ARRAY) return NULL;

    int n = toks[0].size;
    Game *list = calloc(n, sizeof(Game));
    int idx = 0;
    int i = 1;
    for (int j = 0; j < n; j++) {
        int objsize = toks[i].size;
        i++;
        Game g = {0};
        for (int k = 0; k < objsize; k++) {
            jsmntok_t *key = &toks[i++];
            jsmntok_t *val = &toks[i++];
            if (jsoneq(json, key, "id")) {
                g.id = tokcpy(json, val);
            } else if (jsoneq(json, key, "title")) {
                g.title = tokcpy(json, val);
            }
        }
        list[idx++] = g;
    }
    *outCount = n;
    return list;
}

Game parse_game_details(const char *json) {
    jsmn_parser p; jsmntok_t toks[256];
    jsmn_init(&p);
    int ntok = jsmn_parse(&p, json, strlen(json), toks, 256);
    Game g = {0};
    if (ntok < 1 || toks[0].type != JSMN_OBJECT) return g;

    int n = toks[0].size;
    int i = 1;
    for (int k = 0; k < n; k++) {
        jsmntok_t *key = &toks[i++];
        jsmntok_t *val = &toks[i++];
        if      (jsoneq(json, key, "id"))          g.id          = tokcpy(json, val);
        else if (jsoneq(json, key, "title"))       g.title       = tokcpy(json, val);
        else if (jsoneq(json, key, "description")) g.description = tokcpy(json, val);
        else if (jsoneq(json, key, "file_url"))    g.file_url    = tokcpy(json, val);
    }
    return g;
}

void free_game_list(Game *games, int count) {
    for (int i = 0; i < count; i++) {
        free(games[i].id);
        free(games[i].title);
    }
    free(games);
}
