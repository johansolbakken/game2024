#include <string.h>
#define NOB_IMPLEMENTATION
#include "src/nob.h"

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} string_dynamic_array_t;

int main(void) {
    string_dynamic_array_t array = {0};

    DYNAMIC_ARRAY_APPEND(array, "cc");
    DYNAMIC_ARRAY_APPEND(array, "-o compiler");
    DYNAMIC_ARRAY_APPEND(array, "-Wall");
    DYNAMIC_ARRAY_APPEND(array, "-Wextra");
    DYNAMIC_ARRAY_APPEND(array, "-Wpedantic");
    DYNAMIC_ARRAY_APPEND(array, "-Werror");
    DYNAMIC_ARRAY_APPEND(array, "-std=c99");
    DYNAMIC_ARRAY_APPEND(array, "-g");
    DYNAMIC_ARRAY_APPEND(array, "-O0");
    DYNAMIC_ARRAY_APPEND(array, "-Isrc");
    DYNAMIC_ARRAY_APPEND(array, "src/main.c");
    DYNAMIC_ARRAY_APPEND(array, "src/lexer.c");

    char command[1024] = {0};
    for (size_t i = 0; i < array.count; ++i) {
        strcat(command, array.items[i]);
        strcat(command, " ");
    }

    system(command);

    DYNAMIC_ARRAY_FREE(array);

    return 0;
}
