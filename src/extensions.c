#include "libft_malloc_internal.h"

size_t malloc_usable_size(void *ptr) {
    lock_mutex();
    size_t ret = 0;
    if (!mmanager.is_initialized || !ptr)
        goto ret;

    mchunk_t* chunk = find_chunk(ptr);
    if (chunk) {
        if (chunk->size <= TINY_MALLOC_SIZE)
            ret = TINY_MALLOC_SIZE;
        else
            ret = SMALL_MALLOC_SIZE;
        goto ret;
    }

    mzone_no_chunk_t* large_zone = find_large_zone(ptr);
    if (large_zone) {
        ret = large_zone->size;
    }

    ret:
        unlock_mutex();
        return ret;
}