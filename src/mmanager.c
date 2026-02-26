#include "libft_malloc.h"

// rounds up a to a multiple of b (allows to align memory to 16 for exemple)
static size_t round_up_to(size_t a, size_t b) {
    return b * ((a + b - 1) / b);
}

bool initialize_mmanager() {
    // tiny malloc zones
    int i = 0;
    mzone_t** zone_ptr = &mmanager.tiny_malloc_zones;
    size_t malloc_size = TINY_MALLOC_SIZE;
    size_t aligned_struct_size = round_up_to(sizeof(mzone_t), 16);
    size_t zone_size = round_up_to(aligned_struct_size + malloc_size * MALLOC_PER_ZONE, PAGE_SIZE);
    while (i < TINY_MALLOC_ZONE_PREALLOCATED) {

        // memory zone allocation
        mzone_t* mzone = mmap(
            NULL,
            zone_size,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1,
            0
        );

        if (!mzone)
            return false;

        mzone->addr = (void*)((char*)mzone + aligned_struct_size);
        mzone->is_full = false;
        void* start_zone_addr = mzone->addr;
        for (int j = 0; j < MALLOC_PER_ZONE; ++j) {
            mzone->chunks[j].addr = start_zone_addr;
            start_zone_addr += malloc_size;
        }

        *zone_ptr = mzone;
        zone_ptr = &mzone->next;
        ++i;
    }

    // small malloc zones
    i = 0;
    zone_ptr = &mmanager.small_malloc_zones;
    malloc_size = SMALL_MALLOC_SIZE;
    zone_size = round_up_to(aligned_struct_size + malloc_size * MALLOC_PER_ZONE, PAGE_SIZE);
    while (i < SMALL_MALLOC_ZONE_PREALLOCATED) {
        // memory zone allocation
        mzone_t* mzone = mmap(
            NULL,
            zone_size,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1,
            0
        );

        if (!mzone)
            return false;

        mzone->addr = (void*)((char*)mzone + aligned_struct_size);
        mzone->is_full = false;
        void* start_zone_addr = mzone->addr;
        for (int j = 0; j < MALLOC_PER_ZONE; ++j) {
            mzone->chunks[j].addr = start_zone_addr;
            start_zone_addr += malloc_size;
        }

        *zone_ptr = mzone;
        zone_ptr = &mzone->next;
        ++i;
    }

    // large malloc zones are allocated when needed
    mmanager.large_malloc_zones = NULL;

    mmanager.is_initialized = true;
    return true;
}

mchunk_t* get_first_available_chunk(mzone_t* zone) {
    while (zone) {
        if (!zone->is_full) {
            for (int i = 0; i < MALLOC_PER_ZONE; ++i) {
                mchunk_t* chunk = &zone->chunks[i];
                if (!chunk->in_use)
                    return chunk;
            }
            zone->is_full = true;
        }
        zone = zone->next;
    }
    // here we could allocate a new zone if all are full
    return NULL;
}

static mzone_no_chunk_t* malloc_large_zone(size_t size) {
    size_t aligned_struct_size = round_up_to(sizeof(mzone_no_chunk_t), 16);
    size_t zone_size = round_up_to(aligned_struct_size + size, PAGE_SIZE);
    mzone_no_chunk_t* zone = mmap(
        NULL,
        zone_size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );
    if (!zone)
        return NULL;

    zone->addr = (void*)((char*)zone + aligned_struct_size);
    zone->size = size;
    zone->next = NULL;
    return (zone);
}

mzone_no_chunk_t* malloc_and_get_zone(mzone_no_chunk_t** start_zone, size_t size) {
    if (!*start_zone) {
        *start_zone = malloc_large_zone(size);
        if (!start_zone)
            return NULL;
        return *start_zone;
    } else {
        while ((*start_zone)->next) {
            start_zone = &(*start_zone)->next;
        }

        (*start_zone)->next = malloc_large_zone(size);
        if ((*start_zone)->next)
            return NULL;
        return (*start_zone)->next;
    }
}
