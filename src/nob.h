#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define DYNAMIC_ARRAY_APPEND(array, item) \
    do { \
        if ((array).count == (array).capacity) { \
            (array).capacity = (array).capacity ? (array).capacity * 2 : 4; \
            (array).items = realloc((array).items, sizeof(*(array).items) * (array).capacity); \
        } \
        (array).items[(array).count++] = (item); \
    } while (0)

#define DYNAMIC_ARRAY_FREE(array) \
    do { \
        free((array).items); \
        (array).items = NULL; \
        (array).count = 0; \
        (array).capacity = 0; \
    } while (0)

typedef struct {
    char *base;
    size_t size;
    size_t capacity;
} arena_t;

void* arena_alloc(arena_t *arena, size_t size);
void arena_free(arena_t *arena);

typedef struct {
    char *items;
    size_t count;
} nob_string_view_t;

nob_string_view_t nob_string_view_from_cstr(const char *cstr);

typedef struct {
    char *items;
    size_t count;
    size_t capacity;
} nob_string_t;

 bool nob_read_entire_file(const char *filename, nob_string_t* string);

#ifdef NOB_IMPLEMENTATION
void* arena_alloc(arena_t *arena, size_t size) {
    if (arena->size + size > arena->capacity) {
        arena->capacity = arena->capacity * 2 + size;
        arena->base = (char*)realloc(arena->base, arena->capacity);
    }

    void *result = arena->base + arena->size;
    arena->size += size;
    return result;
}

void arena_free(arena_t *arena) {
    free(arena->base);
    arena->base = NULL;
    arena->size = 0;
    arena->capacity = 0;
}

nob_string_view_t nob_string_view_from_cstr(const char *cstr) {
    nob_string_view_t view;
    view.items = (char*)cstr;
    view.count = strlen(cstr);
    return view;
}

bool nob_read_entire_file(const char *filename, nob_string_t* string) {
    (void) string;
    errno = 0;
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Failed to open file '%s': %s\n", filename, strerror(errno));
        return false;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        printf("Failed to seek to end of file '%s': %s\n", filename, strerror(errno));
        fclose(file);
        return false;
    }

    long size = ftell(file);
    if (size == -1L) {
        printf("Failed to get size of file '%s': %s\n", filename, strerror(errno));
        fclose(file);
        return false;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        printf("Failed to seek to start of file '%s': %s\n", filename, strerror(errno));
        fclose(file);
        return false;
    }

    string->items = (char*)malloc(size + 1);
    if (!string->items) {
        printf("Failed to allocate memory for file '%s': %s\n", filename, strerror(errno));
        fclose(file);
        return false;
    }

    size_t read = fread(string->items, 1, size, file);
    if (read != (size_t)size) {
        printf("Failed to read file '%s': %s\n", filename, strerror(errno));
        fclose(file);
        return false;
    }

    string->items[size] = '\0';
    string->count = size;
    fclose(file);

    return true;
}

#endif // NOB_IMPLEMENTATION
