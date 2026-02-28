#include "libft_malloc_internal.h"

void* realloc(void* ptr, size_t size) {
    if (!mmanager.is_initialized)
        return NULL;

    mchunk_t* chunk = find_chunk(ptr);
    if (chunk) {
        char* new_ptr = malloc(size);
        if (!new_ptr)
            return NULL;
        for (size_t i = 0; i < chunk->size; ++i)
            new_ptr[i] = ((char*)chunk->addr)[i];
        chunk->in_use = false;
        chunk->size = 0;
        return (void*)new_ptr;
    }
    mzone_no_chunk_t* large_zone = find_large_zone(ptr);
    if (large_zone) {
        char* new_ptr = malloc(size);
        if (!new_ptr)
            return NULL;
        for (size_t i = 0; i < large_zone->size; ++i)
            new_ptr[i] = ((char*)large_zone->addr)[i];
        free(ptr);
        return (void*)new_ptr;
    }
    return NULL;
}