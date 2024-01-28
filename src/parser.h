#pragma once

#include "lexer.h"

typedef struct {
    lexer_t *lexer;
    token_t *current_token;
} parser_t;

typedef enum {
    NODE_TYPE_PROGRAM,
} node_type_t;

typedef struct node_t {
    node_type_t type;
    token_t token;
    struct node_t** items;
    size_t count;
    size_t capacity;
} node_t;

node_t* parse_program(parser_t *parser);
