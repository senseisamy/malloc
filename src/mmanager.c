#include "libft_malloc.h"

bool initialize_mmanager(mmanager_t* mmanager) {
    if (!mmanager)
        return false;

    ft_bzero(mmanager, sizeof(*mmanager));

    int i = 0;
    mzone_t** zone_ptr = &mmanager->tiny_malloc_zones;
    while (i < TINY_MALLOC_ZONE_PREALLOCATED) {
        // memory zone allocation
        mzone_t* mzone = mmap(
            NULL,
            sizeof(mzone_t) + TINY_MALLOC_SIZE * MALLOC_PER_ZONE,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1,
            0
        );

        mzone->addr = mzone; // useful?
        mzone->is_full = false;
        void* start_zone_addr = mzone + sizeof(mzone_t);
        for (int j = 0; j < MALLOC_PER_ZONE; ++j) {
            mzone->chunks[j].addr = start_zone_addr;
            start_zone_addr += sizeof(TINY_MALLOC_SIZE);
        }

        *zone_ptr = mzone;
        zone_ptr = &mzone->next;
        ++i;
    }
    return true;
}