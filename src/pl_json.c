/*
Copyright (c) 2024, Dominic Szablewski - https://phoboslab.org
SPDX-License-Identifier: MIT

PL_JSON - Yet another single header json parser

Adapted for the current codebase by xfnty.
*/

#include "pl_json.h"

#define JSON_MAX_DEPTH 256

typedef struct {
    json_error_t error;
    char *data;
    unsigned int data_len;
    unsigned int pos;
    json_token_t *tokens;
    unsigned int tokens_capacity;
    unsigned int tokens_len;
    unsigned int tokens_pos;
    unsigned int parsed_size;
    unsigned char *parsed_data;
    unsigned int parsed_data_len;
} json_parser_t;

enum {
    JSON_C_NULL  = (1<<0),
    JSON_C_SPACE = (1<<1),
    JSON_C_LF    = (1<<2),
    JSON_C_NUM   = (1<<3),
    JSON_C_EXP   = (1<<5),
    JSON_C_PRIM  = (1<<6),
    JSON_C_OBJ   = (1<<7)
};

static unsigned char json_char_map[256];
static bool json_static_initialized = false;

#define json_char_is(C, TYPE) (json_char_map[(unsigned char)(C)] & (TYPE))

static void json_init_static_data() {
    json_char_map['\0'] = JSON_C_NULL;
    json_char_map['\t'] = JSON_C_SPACE;
    json_char_map['\n'] = JSON_C_LF;
    json_char_map['\r'] = JSON_C_SPACE;
    json_char_map[' ']  = JSON_C_SPACE;
    json_char_map['+']  = JSON_C_EXP;
    json_char_map['-']  = JSON_C_NUM;
    json_char_map['.']  = JSON_C_EXP;
    json_char_map['0']  = JSON_C_NUM;
    json_char_map['1']  = JSON_C_NUM;
    json_char_map['2']  = JSON_C_NUM;
    json_char_map['3']  = JSON_C_NUM;
    json_char_map['4']  = JSON_C_NUM;
    json_char_map['5']  = JSON_C_NUM;
    json_char_map['6']  = JSON_C_NUM;
    json_char_map['7']  = JSON_C_NUM;
    json_char_map['8']  = JSON_C_NUM;
    json_char_map['9']  = JSON_C_NUM;
    json_char_map['n']  = JSON_C_PRIM;
    json_char_map['t']  = JSON_C_PRIM;
    json_char_map['f']  = JSON_C_PRIM;
    json_char_map['"']  = JSON_C_OBJ;
    json_char_map['[']  = JSON_C_OBJ;
    json_char_map['{']  = JSON_C_OBJ;
    json_char_map['E']  = JSON_C_EXP;
    json_char_map['e']  = JSON_C_EXP;
    json_static_initialized = true;
}

static char json_next(json_parser_t *parser) {
    if (parser->pos >= parser->data_len) {
        return '\0';
    }
    return parser->data[parser->pos++];
}

static char json_next_non_whitespace(json_parser_t *parser) {
    char c;
    do {
        c = json_next(parser);
    } while (json_char_is(c, JSON_C_SPACE | JSON_C_LF));
    return c;
}

static json_token_t *json_tokenize_descent(json_parser_t *parser, unsigned int depth) {
    char c;
    json_token_t *token, *key;

    if (!json_static_initialized) {
        json_init_static_data();
    }

    if (depth > JSON_MAX_DEPTH) {
        parser->error = JSON_ERROR_MAX_DEPTH;
        return 0;
    }
    if (parser->tokens_len >= parser->tokens_capacity) {
        parser->error = JSON_ERROR_MAX_TOKENS;
        return 0;
    }

    parser->parsed_size += sizeof(json_t);
    c = json_next_non_whitespace(parser);

    token = &parser->tokens[parser->tokens_len++];
    token->pos = parser->pos - 1;
    token->len = 0;

    if (json_char_is(c, JSON_C_OBJ)) {
        if (c == '"') {
            token->type = JSON_STRING;
            token->pos++;
            c = json_next(parser);
            while (c != '"') {
                if (c == '\\') {
                    c = json_next(parser);
                }
                else if (json_char_is(c, JSON_C_NULL | JSON_C_LF)) {
                    return 0;
                }
                c = json_next(parser);
            }
            token->len = parser->pos - token->pos - 1;
            parser->parsed_size += token->len + 1;
            return token;
        }
        else if (c == '{') {
            token->type = JSON_OBJECT;
            c = json_next_non_whitespace(parser);
            if (c == '}') {
                return token;
            }
            while (1) {
                parser->pos--;
                token->len++;
                key = json_tokenize_descent(parser, depth+1);
                if (!key || key->type != JSON_STRING) {
                    return 0;
                }
                c = json_next_non_whitespace(parser);
                if (c != ':') {
                    return 0;
                }
                if (!json_tokenize_descent(parser, depth+1)) {
                    return 0;
                }
                c = json_next_non_whitespace(parser);
                if (c == ',') {
                    c = json_next_non_whitespace(parser);
                }
                else if (c == '}') {
                    return token;
                }
                else {
                    return 0;
                }
            }
        }
        else /* if (c == '[') */ {
            token->type = JSON_ARRAY;
            c = json_next_non_whitespace(parser);
            if (c == ']') {
                return token;
            }
            while (1) {
                parser->pos--;
                token->len++;
                if (!json_tokenize_descent(parser, depth+1)) {
                    return 0;
                }
                c = json_next_non_whitespace(parser);
                if (c == ',') {
                    c = json_next_non_whitespace(parser);
                }
                else if (c == ']') {
                    return token;
                }
                else {
                    return 0;
                }
            }
        }
    }
    else if (json_char_is(c, JSON_C_PRIM)) {
        if (c == 'n') {
            token->type = JSON_NULL;
            parser->pos += 3;
        }
        else if (c == 't') {
            token->type = JSON_TRUE;
            parser->pos += 3;
        }
        else /* if (c == 'f') */ {
            token->type = JSON_FALSE;
            parser->pos += 4;
        }
        return token;
    }
    else if (json_char_is(c, JSON_C_NUM)) {
        token->type = JSON_NUMBER;
        do {
            c = json_next(parser);
        } while (json_char_is(c, JSON_C_NUM | JSON_C_EXP));
        parser->pos--;
        token->len = parser->pos - token->pos;
        if (token->len > 63) {
            return 0;
        }
        return token;
    }

    return 0;
}

static void *json_bump_alloc(json_parser_t *parser, unsigned int size) {
    void *p = parser->parsed_data + parser->parsed_data_len;
    parser->parsed_data_len += size;
    return p;
}

static unsigned int json_parse_string(char *dst, char *src, unsigned int len) {
    unsigned int si, di = 0;
    char c;
    for (si = 0; si < len; si++, di++) {
        c = src[si];
        if (c == '\\') {
            c = src[++si];
            switch (c) {
                case 'r': c = '\r'; break;
                case 'n': c = '\n'; break;
                case 'b': c = '\b'; break;
                case 'f': c = '\f'; break;
                case 't': c = '\t'; break;
                case 'u': c = '?'; si += 4; break;
            }
        }
        dst[di] = c;
    }
    dst[di] = '\0';
    return di;
}

static void json_parse_descent(json_parser_t *parser, json_t *v) {
    char buf[64];
    char *str, **keys;
    json_t *values;
    unsigned int i;

    json_token_t *t = &parser->tokens[parser->tokens_pos++];
    v->type = t->type;

    if (v->type == JSON_NULL) {
        v->value.number = 0;
    }
    else if (v->type == JSON_TRUE) {
        v->value.number = 1;
    }
    else if (v->type == JSON_FALSE) {
        v->value.number = 0;
    }
    else if (v->type == JSON_NUMBER) {
        memcpy((void *)buf, &parser->data[t->pos], t->len);
        buf[t->len] = '\0';
        v->value.number = atof(buf);
    }
    else if (v->type == JSON_STRING) {
        str = json_bump_alloc(parser, t->len + 1);
        v->len = json_parse_string(str, &parser->data[t->pos], t->len);
        v->value.string = str;
    }
    else if (v->type == JSON_OBJECT) {
        values = json_bump_alloc(parser, sizeof(json_t) * t->len);
        keys = json_bump_alloc(parser, sizeof(char*) * t->len);
        v->value.values = values;
        v->len = t->len;

        for (i = 0; i < t->len; i++) {
            json_token_t *key = &parser->tokens[parser->tokens_pos++];
            keys[i] = json_bump_alloc(parser, key->len + 1);
            json_parse_string(keys[i], &parser->data[key->pos], key->len);
            json_parse_descent(parser, &values[i]);
        }
    }
    else if (v->type == JSON_ARRAY) {
        values = json_bump_alloc(parser, sizeof(json_t) * t->len);
        v->len = t->len;
        v->value.values = values;
        for (i = 0; i < t->len; i++) {
            json_parse_descent(parser, &values[i]);
        }
    }
}

int json_tokenize(char *data, unsigned int len, json_token_t *tokens, unsigned int tokens_len, unsigned int *parsed_size_req) {
    json_parser_t parser;

    parser.error = JSON_ERROR_INVALID;
    parser.data = data;
    parser.data_len = len;
    parser.tokens = tokens;
    parser.tokens_capacity = tokens_len;

    if (!json_tokenize_descent(&parser, 0)) {
        return parser.error;
    }

    *parsed_size_req = parser.parsed_size;
    return parser.tokens_len;
}

void json_parse_tokens(char *data, json_token_t *tokens, unsigned int tokens_len, json_t *json) {
    json_parser_t parser;
    RtlZeroMemory(&parser, sizeof(parser));
    parser.data = data;
    parser.tokens = tokens;
    parser.tokens_len = tokens_len;
    parser.parsed_data = (unsigned char *)json;
    json_parse_descent(&parser, json_bump_alloc(&parser, sizeof(json_t)));
}

double json_number(json_t *v) {
    if (!v || v->type > JSON_NUMBER) {
        return 0;
    }
    return v->value.number;
}

int json_bool(json_t *v) {
    if (!v) {
        return 0;
    }
    if (v->type > JSON_NUMBER) {
        return (v->len > 0);
    }
    return (v->value.number != 0);
}

char *json_string(json_t *v) {
    if (!v || v->type != JSON_STRING) {
        return 0;
    }
    return v->value.string;
}

json_t *json_values(json_t *v) {
    if (!v || v->len == 0 || v->type < JSON_ARRAY) {
        return 0;
    }
    return v->value.values;
}

json_t *json_value_at(json_t *v, unsigned int i) {
    if (!v || i >= v->len || v->type < JSON_ARRAY) {
        return 0;
    }
    return v->value.values + i;
}

char **json_keys(json_t *v) {
    if (!v || v->len == 0 || v->type != JSON_OBJECT) {
        return 0;
    }
    return (char **)(v->value.values + v->len);
}

char *json_key_at(json_t *v, unsigned int i) {
    if (!v || i >= v->len || v->type != JSON_OBJECT) {
        return 0;
    }
    return (char *)(((char **)(v->value.values + v->len)) + i);
}

json_t *json_value_for_key(json_t *v, char *key) {
    char **keys;
    unsigned int i;
    if (!v || v->type != JSON_OBJECT) {
        return 0;
    }
    keys = json_keys(v);
    for (i = 0; i < v->len; i++) {
        if (strcmp(keys[i], key) == 0) {
            return json_value_at(v, i);
        }
    }
    return 0;
}
