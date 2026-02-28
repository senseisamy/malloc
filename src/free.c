#include "libft_malloc_internal.h"

void free(void *ptr) {
    if (!mmanager.is_initialized)
        return;

    mchunk_t* chunk = find_chunk(ptr);
    if (chunk) {
        chunk->in_use = false;
        chunk->size = 0;
        return;
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
            if (prev_zone == NULL)
                return;
            prev_zone->next = large_zone->next; // removes the zone from the chained list
        }

        size_t aligned_struct_size = round_up_to(sizeof(mzone_no_chunk_t), 16);
        munmap(large_zone, round_up_to(aligned_struct_size + large_zone->size, PAGE_SIZE));
    }
}