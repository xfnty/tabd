/*
Copyright (c) 2024, Dominic Szablewski - https://phoboslab.org
SPDX-License-Identifier: MIT

PL_JSON - Yet another single header json parser

Adapted for the current codebase by xfnty.
*/

#ifndef PL_JSON_H
#define PL_JSON_H

#include "std.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    JSON_NULL,
    JSON_TRUE,
    JSON_FALSE,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} json_type_t;

typedef struct {
    json_type_t type;
    unsigned int pos;
    unsigned int len;
} json_token_t;

typedef struct json_t {
    json_type_t type;
    unsigned int len;
    union {
        double number;
        char *string;
        struct json_t *values;
    } value;
} json_t;

typedef enum {
    JSON_ERROR_INVALID    = -1,
    JSON_ERROR_MAX_TOKENS = -2,
    JSON_ERROR_MAX_DEPTH  = -3
} json_error_t;

int json_tokenize(char *data, unsigned int len, json_token_t *tokens, unsigned int tokens_len, unsigned int *parsed_size_req);
void json_parse_tokens(char *data, json_token_t *tokens, unsigned int len, json_t *json);
double json_number(json_t *v);
int json_bool(json_t *v);
char *json_string(json_t *v);
json_t *json_values(json_t *v);
json_t *json_value_at(json_t *v, unsigned int i);
char **json_keys(json_t *v);
char *json_key_at(json_t *v, unsigned int i);
json_t *json_value_for_key(json_t *v, char *key);

#ifdef __cplusplus
}
#endif
#endif /* PL_JSON_H */
