#include "libft_malloc_internal.h"

bool free_internal(void *ptr) {
    if (!mmanager.is_initialized)
        return false;
    if (!ptr)
        return true;

    mchunk_t* chunk = find_chunk(ptr);
    if (chunk) {
        chunk->in_use = false;
        chunk->size = 0;
        if (mmanager.debug.count_alloc)
            ++mmanager.debug.n_free;
        return true;
    }

    mzone_no_chunk_t* large_zone = find_large_zone(ptr);
    if (large_zone) {
        if (large_zone == mmanager.large_malloc_zones) {
            mmanager.large_malloc_zones = large_zone->next;
        } else {
            mzone_no_chunk_t* prev_zone = NULL;
            prev_zone = mmanager.large_malloc_zones;
            while (prev_zone->next && prev_zone->next != large_zone) {
                prev_zone = prev_zone->next;
            }
            if (prev_zone == NULL) {
                return false;
            }
            prev_zone->next = large_zone->next; // removes the zone from the chained list
        }

        size_t aligned_struct_size = round_up_to(sizeof(mzone_no_chunk_t), 16);
        munmap(large_zone, round_up_to(aligned_struct_size + large_zone->size, PAGE_SIZE));
        if (mmanager.debug.count_alloc)
            ++mmanager.debug.n_free;
        return true;
    }

    return false;
}

void free(void* ptr) {
    lock_mutex();

    if (!mmanager.is_initialized)
        initialize_mmanager();

    bool ret = free_internal(ptr);

    switch (mmanager.debug.log_level) {
        case DEBUG:
            if (ret)
                ft_printf("call to %sfree(%p)%s, %ssuccess%s\n", ITALIC, ptr, RESET, GREEN, RESET);
        case WARNING:
            if (!ret)
                ft_printf("call to %sfree(%p)%s, %sfailed%s\n", ITALIC, ptr, RESET, RED, RESET);
        case ERROR:
            if (!mmanager.is_initialized)
                ft_printf("%sFailed to initialize mmanager%s\n", RED, RESET);
        case NONE:
    }

    unlock_mutex();
}
