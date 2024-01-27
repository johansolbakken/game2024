#include "lexer.h"
#include "nob.h"

#include <ctype.h>

nob_string_view_t lexer_next_token(lexer_t *lexer)
{
    while (lexer->source.count > 0 && isspace(lexer->source.start[0])) {
        lexer->source.start++;
        lexer->source.count--;
    }

    if (lexer->source.count == 0) {
        return lexer->source;
    }

    nob_string_view_t token = {0};
    token.start = lexer->source.start;
    token.count = 0;
    while (token.count < lexer->source.count && !isspace(token.start[token.count])) {
        token.count++;
    }
    lexer->source.start += token.count;
    lexer->source.count -= token.count;

    return token;
}
