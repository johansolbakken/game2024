#define NOB_IMPLEMENTATION
#include "nob.h"

#include "lexer.h"

int main(int argc, char**argv) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    nob_string_t source = {0};
    if (!nob_read_entire_file(argv[1], &source)) return 1;

    printf("%s\n", source.items);

    return 0;
}
