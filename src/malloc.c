#include "libft_malloc.h"

void* malloc(size_t size) {
    if (!mmanager.is_initialized)
        if (!initialize_mmanager())
            return NULL;

    if (size == 0)
        return NULL;
    if (size <= (size_t)SMALL_MALLOC_SIZE) { // tiny or small malloc
        mchunk_t* chunk = NULL;
        if (size <= (size_t)TINY_MALLOC_SIZE)
            chunk = get_first_available_chunk(mmanager.tiny_malloc_zones);
        else
            chunk = get_first_available_chunk(mmanager.small_malloc_zones);

        if (!chunk)
            return NULL;

        chunk->in_use = true;
        chunk->size = size;
        return chunk->addr;
    }
    else { // large malloc
        mzone_no_chunk_t* zone = malloc_and_get_zone(&mmanager.large_malloc_zones, size);
        if (!zone)
            return NULL;
        return (zone->addr);
    }
}