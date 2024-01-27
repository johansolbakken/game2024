#pragma once

#include <stdio.h>
#include <stdlib.h>

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
