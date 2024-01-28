#include "lexer.h"
#include "nob.h"

#include <ctype.h>

#include "keywords.c"

token_t lexer_next_token(lexer_t *lexer)
{
    while (lexer->source.count > 0 && isspace(lexer->source.start[0])) {
        lexer->source.start++;
        lexer->source.count--;
    }

    if (lexer->source.count > 1 && (strncmp(lexer->source.start, "//", 2) == 0)) {
        while (lexer->source.count > 0 && lexer->source.start[0] != '\n') {
            lexer->source.start++;
            lexer->source.count--;
        }
    }

    if (lexer->source.count == 0) {
        return (token_t){0};
    }

    nob_string_view_t lexeme = {0};
    lexeme.start = lexer->source.start;

    if (strncmp(lexeme.start, "#", 1) == 0) {
        while (lexer->source.count > 0 && lexer->source.start[0] != '\n') {
            lexer->source.start++;
            lexer->source.count--;
        }

        lexeme.count = lexer->source.start - lexeme.start;

        return (token_t){.type = TOKEN_PREPROCESSOR, .lexeme = lexeme};
    }

    if (isalpha(lexer->source.start[0]) || lexer->source.start[0] == '_') {
        while (lexer->source.count > 0 && (isalnum(lexer->source.start[0]) || lexer->source.start[0] == '_')) {
            lexer->source.start++;
            lexer->source.count--;
        }

        lexeme.count = lexer->source.start - lexeme.start;

        for (size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i) {
            if (strncmp(lexeme.start, keywords[i], lexeme.count) == 0) {
                return (token_t){.type = i + 1, .lexeme = lexeme};
            }
        }

        return (token_t){.type = TOKEN_IDENTIFIER, .lexeme = lexeme};
    }

    if (isdigit(lexer->source.start[0])) {
        while (lexer->source.count > 0 && isdigit(lexer->source.start[0])) {
            lexer->source.start++;
            lexer->source.count--;
        }

        lexeme.count = lexer->source.start - lexeme.start;

        return (token_t){.type = TOKEN_NUMBER_LITERAL, .lexeme = lexeme};
    }

    if (lexer->source.start[0] == '"') {
        lexer->source.start++;
        lexer->source.count--;

        while (lexer->source.count > 0 && lexer->source.start[0] != '"') {
            lexer->source.start++;
            lexer->source.count--;
        }

        if (lexer->source.count == 0) {
            nob_log(NOB_ERROR, "Unterminated string literal");
            return (token_t){0};
        }

        lexer->source.start++;
        lexer->source.count--;

        lexeme.count = lexer->source.start - lexeme.start;

        return (token_t){.type = TOKEN_STRING_LITERAL, .lexeme = lexeme};
    }

    if (lexer->source.start[0] == '\'') {
        lexer->source.start++;
        lexer->source.count--;

        while (lexer->source.count > 0 && lexer->source.start[0] != '\'') {
            lexer->source.start++;
            lexer->source.count--;
        }

        if (lexer->source.count == 0) {
            nob_log(NOB_ERROR, "Unterminated character literal");
            return (token_t){0};
        }

        lexer->source.start++;
        lexer->source.count--;

        lexeme.count = lexer->source.start - lexeme.start;

        return (token_t){.type = TOKEN_CHARACTER_LITERAL, .lexeme = lexeme};
    }

    token_t token = {0};
    token.lexeme.start = lexer->source.start;
    token.lexeme.count = 1;
    token.type = lexer->source.start[0];

    lexer->source.start++;

    return token;
}
