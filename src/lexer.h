#pragma once

#include "nob.h"

typedef struct {
    int position;
    nob_string_view_t source;
} lexer_t;

nob_string_view_t lexer_next_token(lexer_t *lexer);
