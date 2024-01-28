#define NOB_IMPLEMENTATION
#define NOB_TRACK_ALLOCATIONS
#include "nob.h"

#include "lexer.h"
#include "parser.h"

int main(int argc, char**argv) {
    if (argc < 2) {
        nob_log(NOB_INFO, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    nob_string_t source = {0};
    if (!nob_read_entire_file(argv[1], &source)) return 1;

    nob_string_view_t view = {0};
    view.start = source.items;
    view.count = source.count;

    lexer_t lexer = {0};
    lexer.source = view;

    parser_t parser = {0};
    parser.lexer = &lexer;

    node_t* node = parse_program(&parser);
    (void) node;

    nob_free(source.items);

    nob_print_allocations();

    return 0;
}
