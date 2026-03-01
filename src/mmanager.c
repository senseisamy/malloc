#include "libft_malloc_internal.h"

mmanager_t mmanager;
pthread_mutex_t mutex_malloc = PTHREAD_MUTEX_INITIALIZER;

// rounds up a to a multiple of b (allows to align memory to 16 for exemple)
size_t round_up_to(size_t a, size_t b) {
    return b * ((a + b - 1) / b);
}

mzone_t* malloc_new_zone(size_t chunk_size) {
    size_t aligned_struct_size = round_up_to(sizeof(mzone_t), 16);
    size_t zone_size = round_up_to(aligned_struct_size + chunk_size * MALLOC_PER_ZONE, PAGE_SIZE);
    mzone_t* mzone = mmap(
        NULL,
        zone_size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );
    if (!mzone) 
        return NULL;

    mzone->addr = (void*)((char*)mzone + aligned_struct_size);
    mzone->is_full = false;
    void* start_zone_addr = mzone->addr;
    for (int j = 0; j < MALLOC_PER_ZONE; ++j) {
        mzone->chunks[j].addr = start_zone_addr;
        start_zone_addr += chunk_size;
    }
    return mzone;
}

bool initialize_mmanager() {
    // tiny malloc zones
    int i = 0;
    mzone_t** zone_ptr = &mmanager.tiny_malloc_zones;
    while (i < TINY_MALLOC_ZONE_PREALLOCATED) {
        mzone_t* mzone = malloc_new_zone(TINY_MALLOC_SIZE);
        if (!mzone)
            return false;
        *zone_ptr = mzone;
        zone_ptr = &mzone->next;
        ++i;
    }

    // small malloc zones
    i = 0;
    zone_ptr = &mmanager.small_malloc_zones;
    while (i < SMALL_MALLOC_ZONE_PREALLOCATED) {
        mzone_t* mzone = malloc_new_zone(SMALL_MALLOC_SIZE);
        if (!mzone)
            return false;
        *zone_ptr = mzone;
        zone_ptr = &mzone->next;
        ++i;
    }

    // large malloc zones are allocated when needed
    mmanager.large_malloc_zones = NULL;

    mmanager.is_initialized = true;
    return true;
}

void show_malloc_mem() {
    if (!mmanager.is_initialized) {
        ft_printf("show_malloc_mem: mmanager is not initialized\n");
        return;
    }

    size_t total_memory_in_use = 0;

    mzone_t* tiny = mmanager.tiny_malloc_zones;
    while (tiny) {
        ft_printf("TINY: %p\n", (void*)tiny);
        for (int i = 0; i < MALLOC_PER_ZONE; ++i) {
            mchunk_t* chunk = &tiny->chunks[i];
            if (chunk->in_use) {
                void* start = chunk->addr;
                void* end = (void*)((char*)start + chunk->size);
                ft_printf("%p - %p : %u bytes\n", start, end, chunk->size);
                total_memory_in_use += chunk->size;
            }
        }
        tiny = tiny->next;
    }
    mzone_t* small = mmanager.small_malloc_zones;
    while (small) {
        ft_printf("SMALL: %p\n", (void*)small);
        for (int i = 0; i < MALLOC_PER_ZONE; ++i) {
            mchunk_t* chunk = &small->chunks[i];
            if (chunk->in_use) {
                void* start = chunk->addr;
                void* end = (void*)((char*)start + chunk->size);
                ft_printf("%p - %p : %u bytes\n", start, end, chunk->size);
                total_memory_in_use += chunk->size;
            }
        }
        small = small->next;
    }
    mzone_no_chunk_t* large = mmanager.large_malloc_zones;
    while (large) {
        ft_printf("LARGE: %p\n", (void*)large);
        void* start = large->addr;
        void* end = (void*)((char*)start + large->size);
        ft_printf("%p - %p : %u bytes\n", start, end, large->size);
        total_memory_in_use += large->size;
        large = large->next;
    }
    ft_printf("TOTAL : %u bytes\n", total_memory_in_use);
}

static bool is_in_zone(void* ptr, mzone_t* zone) {
    return ptr >= zone->chunks[0].addr && ptr <= zone->chunks[MALLOC_PER_ZONE - 1].addr;
}

static mchunk_t* search_in_zone(void* ptr, mzone_t* zone) {
    for (int i = 0; i < MALLOC_PER_ZONE; ++i) {
        mchunk_t* chunk = &zone->chunks[i];
        if (chunk->addr == ptr) {
            return chunk;
        }
    }
    return NULL;
}

mchunk_t* find_chunk(void* ptr) {
    if (!mmanager.is_initialized)
        return NULL;

    mzone_t* zone = mmanager.tiny_malloc_zones;
    bool search_in_tiny = true;
    while (zone) {
        if (is_in_zone(ptr, zone)) {
            if (zone->is_full)
                zone->is_full = false;
            return search_in_zone(ptr, zone);
        }
        zone = zone->next;
        if (zone == NULL && search_in_tiny) {
            zone = mmanager.small_malloc_zones;
            search_in_tiny = false;
        }
    }
    return NULL;
}

mzone_no_chunk_t* find_large_zone(void* ptr) {
    if (!mmanager.is_initialized)
        return NULL;

    mzone_no_chunk_t* large_zone = mmanager.large_malloc_zones;

    while (large_zone) {
        if (large_zone->addr == ptr)
            return large_zone;
        large_zone = large_zone->next;
    }
    return NULL;
}

inline void lock_mutex() {
    pthread_mutex_lock(&mutex_malloc);
}

inline void unlock_mutex() {
    pthread_mutex_unlock(&mutex_malloc);
}

inline void* unlock_mutex_and_return(void* ptr) {
    pthread_mutex_unlock(&mutex_malloc);
    if (!ptr)
        ft_printf("returning NULL\n");
    return ptr;
}