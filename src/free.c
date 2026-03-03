#include "libft_malloc_internal.h"

bool free_internal(void *ptr) {
    if (!ptr)
        return true;

    mchunk_t* chunk = find_chunk(ptr);
    if (chunk) {
        chunk->in_use = false;
        chunk->size = 0;
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
        return true;
    }

    return false;
}

void free(void* ptr) {
    lock_mutex();

    bool ret = false;
    if (!mmanager.is_initialized)
        if (!initialize_mmanager())
            goto ret;

    if (mmanager.debug_properties.enable_logs)
        ft_printf("call to %sfree(%p)%s, ", ITALIC, ptr, RESET);
    ret = free_internal(ptr);

    ret:
        if (mmanager.debug_properties.enable_logs) {
            if (!mmanager.is_initialized)
                ft_printf("%sFailed to initialize mmanager%s\n", RED, RESET);
            else
                ft_printf("%s%s\n", (ret ? GREEN"succes" : RED"failed"), RESET);
        }
        unlock_mutex();
}
