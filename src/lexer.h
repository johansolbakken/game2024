#pragma once

#include "nob.h"

typedef struct {
    int position;
    nob_string_view_t source;
} lexer_t;

typedef enum {
    TOKEN_EOF = 0,
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_CONSTANT,
    TOKEN_STRING_LITERAL,
    TOKEN_NUMBER_LITERAL,
    TOKEN_CHARACTER_LITERAL,
    TOKEN_PUNCTUATOR,
    TOKEN_COMMENT,
    TOKEN_PREPROCESSOR,
} token_type_t;

typedef struct {
    nob_string_view_t lexeme;
    int position;
    token_type_t type;
} token_t;

token_t lexer_next_token(lexer_t *lexer);
