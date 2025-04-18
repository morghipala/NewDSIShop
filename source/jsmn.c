/*
 * Copyright (c) 2010 Sergey
 * MIT license
 */
#include "jsmn.h"
#include <string.h>

/* Allocates a fresh unused token from the token pool. */
static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser,
                                   jsmntok_t *tokens,
                                   size_t num_tokens) {
    jsmntok_t *tok;
    if (parser->toknext >= num_tokens) return NULL;
    tok = &tokens[parser->toknext++];
    tok->start = tok->end = tok->size = 0;
    tok->type = JSMN_UNDEFINED;
    tok->parent = -1;
    return tok;
}

static void jsmn_fill_token(jsmntok_t *token, jsmntype_t type,
                            int start, int end) {
    token->type = type;
    token->start = start;
    token->end = end;
    token->size = 0;
}

/* Fills next available token with JSON primitive. */
static int jsmn_parse_primitive(jsmn_parser *parser,
                                const char *js,
                                size_t len,
                                jsmntok_t *tokens,
                                size_t num_tokens) {
    jsmntok_t *token;
    int start = parser->pos;

    for (; parser->pos < len; parser->pos++) {
        switch (js[parser->pos]) {
            case '\t': case '\r': case '\n': case ' ':
            case ',': case ']': case '}':
                goto found;
        }
    }
found:
    if (tokens == NULL) {
        parser->pos--;
        return 0;
    }
    token = jsmn_alloc_token(parser, tokens, num_tokens);
    if (token == NULL) return -1;
    jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
    token->parent = parser->toksuper;
    parser->pos--;
    return 0;
}

/* Fills next token with JSON string. */
static int jsmn_parse_string(jsmn_parser *parser,
                             const char *js,
                             size_t len,
                             jsmntok_t *tokens,
                             size_t num_tokens) {
    jsmntok_t *token;

    int start = parser->pos+1;
    parser->pos++;

    for (; parser->pos < len; parser->pos++) {
        char c = js[parser->pos];
        if (c == '\"') {
            if (tokens == NULL) {
                return 0;
            }
            token = jsmn_alloc_token(parser, tokens, num_tokens);
            if (token == NULL) return -1;
            jsmn_fill_token(token, JSMN_STRING, start, parser->pos);
            token->parent = parser->toksuper;
            return 0;
        }
        if (c == '\\') parser->pos++;
    }
    return -1;
}

/* Create parser and initialize it. */
void jsmn_init(jsmn_parser *parser) {
    parser->pos = 0;
    parser->toknext = 0;
    parser->toksuper = -1;
}

/* Parse JSON string and fill tokens. */
int jsmn_parse(jsmn_parser *parser, const char *js, size_t len,
               jsmntok_t *tokens, unsigned int num_tokens) {
    int r;
    int i;
    jsmntok_t *token;

    for (; parser->pos < len; parser->pos++) {
        char c = js[parser->pos];
        switch (c) {
            case '{': case '[':
                token = jsmn_alloc_token(parser, tokens, num_tokens);
                if (token == NULL) return -1;
                token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
                token->start = parser->pos;
                token->parent = parser->toksuper;
                parser->toksuper = parser->toknext - 1;
                break;

            case '}': case ']':
                {
                jsmntype_t type =
                    (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
                for (i = parser->toknext - 1; i >= 0; i--) {
                    token = &tokens[i];
                    if (token->start != -1 && token->end == -1) {
                        if (token->type != type) return -1;
                        token->end = parser->pos + 1;
                        parser->toksuper = token->parent;
                        break;
                    }
                }
                }
                break;

            case '\"':
                r = jsmn_parse_string(parser, js, len, tokens, num_tokens);
                if (r < 0) return r;
                break;

            case '\t': case '\r': case '\n': case ' ':
                break;

            case ':':
                parser->toksuper = parser->toknext - 1;
                break;

            case ',':
                if (tokens != NULL &&
                    parser->toksuper != -1 &&
                    tokens[parser->toksuper].type != JSMN_ARRAY &&
                    tokens[parser->toksuper].type != JSMN_OBJECT) {
                    parser->toksuper = tokens[parser->toksuper].parent;
                }
                break;

            default:
                r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
                if (r < 0) return r;
                break;
        }
    }

    for (i = parser->toknext - 1; i >= 0; i--) {
        if (tokens[i].start != -1 && tokens[i].end == -1) {
            return -1;
        }
    }

    return parser->toknext;
}
