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
            (array).items = nob_realloc((array).items, sizeof(*(array).items) * (array).capacity); \
        } \
        (array).items[(array).count++] = (item); \
    } while (0)

#define DYNAMIC_ARRAY_FREE(array) \
    do { \
        nob_free((array).items); \
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
    char *start;
    size_t count;
} nob_string_view_t;

nob_string_view_t nob_string_view_from_cstr(const char *cstr);

typedef struct {
    char *items;
    size_t count;
    size_t capacity;
} nob_string_t;

 bool nob_read_entire_file(const char *filename, nob_string_t* string);

void* nob_malloc_p(size_t size
    , const char *file
    , int line);
#define nob_malloc(size) nob_malloc_p(size, __FILE__, __LINE__)
void nob_free(void *ptr);
void* nob_realloc(void *ptr, size_t size);
void nob_print_allocations(void);

#ifdef NOB_IMPLEMENTATION

void* arena_alloc(arena_t *arena, size_t size) {
    if (arena->size + size > arena->capacity) {
        arena->capacity = arena->capacity * 2 + size;
        arena->base = (char*)nob_realloc(arena->base, arena->capacity);
    }

    void *result = arena->base + arena->size;
    arena->size += size;
    return result;
}

void arena_free(arena_t *arena) {
    nob_free(arena->base);
    arena->base = NULL;
    arena->size = 0;
    arena->capacity = 0;
}

nob_string_view_t nob_string_view_from_cstr(const char *cstr) {
    nob_string_view_t view;
    view.start = (char*)cstr;
    view.count = strlen(cstr);
    return view;
}

bool nob_read_entire_file(const char *filename, nob_string_t* string) {
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

    string->items = (char*)nob_malloc(size + 1);
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

typedef struct {
    void* ptr;
    size_t size;
    const char *file;
    int line;
} nob_allocation_t;

typedef struct nob_allocation_dynamic_array_t {
    nob_allocation_t *items;
    size_t count;
    size_t capacity;
} nob_allocation_dynamic_array_t;

nob_allocation_dynamic_array_t allocations = {0};

void* nob_malloc_p(size_t size
    , const char *file
    , int line)
{
    void *ptr = malloc(size);
#ifdef NOB_TRACK_ALLOCATIONS
    if (!ptr) {
        printf("Failed to allocate %zu bytes of memory in file '%s' on line %d: %s\n"
            , size
            , file
            , line
            , strerror(errno));
        return NULL;
    }
    nob_allocation_t allocation = {ptr, size, file, line};
    DYNAMIC_ARRAY_APPEND(allocations, allocation);
#else
   (void)file;
    (void)line;
#endif // NOB_TRACK_ALLOCATIONS
    return ptr;
}

void nob_free(void *ptr)
{
#ifdef NOB_TRACK_ALLOCATIONS
    bool found = false;
    for (size_t i = 0; i < allocations.count; ++i) {
        if (allocations.items[i].ptr == ptr) {
            found = true;
            break;
        }
    }
    if (!found) {
        printf("Attempted to free a pointer that was not allocated by nob_malloc: %p\n", ptr);
        return;
    }
#endif // NOB_TRACK_ALLOCATIONS
    free(ptr);
#ifdef NOB_TRACK_ALLOCATIONS
    for (size_t i = 0; i < allocations.count; ++i) {
        if (allocations.items[i].ptr == ptr) {
            for (size_t j = i; j < allocations.count - 1; ++j) {
                allocations.items[j] = allocations.items[j + 1];
            }
            --allocations.count;
        }
    }
#endif // NOB_TRACK_ALLOCATIONS
}

void* nob_realloc(void *ptr, size_t size)
{
    void *new_ptr = realloc(ptr, size);
#ifdef NOB_TRACK_ALLOCATIONS
    for (size_t i = 0; i < allocations.count; ++i) {
        if (allocations.items[i].ptr == ptr) {
            allocations.items[i].ptr = new_ptr;
        }
    }
#endif // NOB_TRACK_ALLOCATIONS
    return new_ptr;
}

void nob_print_allocations(void)
{
    if (allocations.count == 0) {
        return;
    }
    printf("Allocations:\n");
    for (size_t i = 0; i < allocations.count; ++i) {
        printf("  %p: %zu bytes allocated in file %s:%d\n"
            , allocations.items[i].ptr
            , allocations.items[i].size
            , allocations.items[i].file
            , allocations.items[i].line);
    }
}


#endif // NOB_IMPLEMENTATION
