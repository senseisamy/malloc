#include "libft_malloc_internal.h"

mmanager_t mmanager;
pthread_mutex_t mutex_malloc = PTHREAD_MUTEX_INITIALIZER;
extern char** environ; // global array of environment variables

static bool starts_with(const char* str, const char* prefix) {
    while (*prefix)
        if (*prefix++ != *str++)
            return false;
    return true;
}

static void init_debug_properties(debug_malloc_t* debug) {
    if (!debug)
        return;
    *debug = (debug_malloc_t){0}; // sets every field to false

    if (!environ)
        return;
    for (int i = 0; environ[i]; ++i) {
        if (starts_with(environ[i], "FT_MALLOC_LOGS=DEBUG"))
            debug->log_level = MAX(debug->log_level, DEBUG);
        else if (starts_with(environ[i], "FT_MALLOC_LOGS=WARNING"))
            debug->log_level = MAX(debug->log_level, WARNING);
        else if (starts_with(environ[i], "FT_MALLOC_LOGS=ERROR"))
            debug->log_level = MAX(debug->log_level, ERROR);
        else if (starts_with(environ[i], "FT_MALLOC_DUMP_MEM_ON_EXIT=1"))
            debug->dump_mem_on_exit = true;
        else if (starts_with(environ[i], "FT_MALLOC_COUNT=1"))
            debug->count_alloc = true;
    }
}

// rounds up a to a multiple of b (allows to align memory to 16 for exemple)
size_t round_up_to(size_t a, size_t b) {
    return b * ((a + b - 1) / b);
}

mzone_t* malloc_new_zone(size_t chunk_size) {
    size_t aligned_struct_size = round_up_to(sizeof(mzone_t), 16);
    size_t zone_size = round_up_to(aligned_struct_size + chunk_size * CHUNKS_PER_ZONE, PAGE_SIZE);
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
    mzone->size = zone_size;
    mzone->is_full = false;
    void* start_zone_addr = mzone->addr;
    for (int j = 0; j < CHUNKS_PER_ZONE; ++j) {
        mzone->chunks[j].addr = start_zone_addr;
        start_zone_addr += chunk_size;
    }
    return mzone;
}

bool initialize_mmanager() {
    init_debug_properties(&mmanager.debug);

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

bool is_empty(mzone_t* zone) {
    for (int i = 0; i < CHUNKS_PER_ZONE; ++i)
        if (zone->chunks[i].in_use)
            return false;
    return true;
}

void show_malloc_mem() {
    lock_mutex();

    if (!mmanager.is_initialized) {
        ft_printf("show_malloc_mem: mmanager is not initialized\n");
        return;
    }

    size_t total_memory_in_use = 0;

    int zone_count = 1;
    mzone_t* tiny = mmanager.tiny_malloc_zones;
    while (tiny) {
        if (!is_empty(tiny)) {
            ft_printf("%sTINY ZONE %d:%s %p->%p%s\n", BOLD, zone_count, RESET ITALIC, (void*)tiny, (char*)tiny + tiny->size, RESET);
            for (int i = 0; i < CHUNKS_PER_ZONE; ++i) {
                mchunk_t* chunk = &tiny->chunks[i];
                if (chunk->in_use) {
                    void* start = chunk->addr;
                    void* end = (void*)((char*)start + chunk->size);
                    ft_printf(" chunk %d:\t%s%p->%p%s (%u bytes)\n", i + 1, ITALIC, start, end, RESET, chunk->size);
                    total_memory_in_use += chunk->size;
                }
            }
        }
        ++zone_count;
        tiny = tiny->next;
    }
    zone_count = 1;
    mzone_t* small = mmanager.small_malloc_zones;
    while (small) {
        if (!is_empty(small)) {
            ft_printf("%sSMALL ZONE %d:%s %p - %p%s\n", BOLD, zone_count, RESET ITALIC, (void*)small, (char*)small + small->size, RESET);
            for (int i = 0; i < CHUNKS_PER_ZONE; ++i) {
                mchunk_t* chunk = &small->chunks[i];
                if (chunk->in_use) {
                    void* start = chunk->addr;
                    void* end = (void*)((char*)start + chunk->size);
                    ft_printf(" chunk %d:\t%s%p->%p%s (%u bytes)\n", i + 1, ITALIC, start, end, RESET, chunk->size);
                    total_memory_in_use += chunk->size;
                }
            }
        }
        ++zone_count;
        small = small->next;
    }
    zone_count = 1;
    mzone_no_chunk_t* large = mmanager.large_malloc_zones;
    while (large) {
        void* start = large->addr;
        void* end = (void*)((char*)start + large->size);
        ft_printf("%sLARGE ZONE %d:\t%s%p->%p%s (%u bytes)\n", BOLD, zone_count, RESET ITALIC, start, end, RESET, large->size);
        total_memory_in_use += large->size;
        ++zone_count;
        large = large->next;
    }
    ft_printf("%sTOTAL:%s %u bytes allocated\n", UNDERLINE BOLD, RESET, total_memory_in_use);
    if (mmanager.debug.count_alloc)
        ft_printf("%u allocations, %d free\n", mmanager.debug.n_alloc, mmanager.debug.n_free);

    unlock_mutex();
}

// function that gets called when the program exits
__attribute__((destructor))
static void dump_memory_on_exit() {
    if (!mmanager.debug.dump_mem_on_exit)
        return;

    lock_mutex();
    // when this function is called FD 1 may already be closed so we have to open it again in order to print
    int tty_fd = open("/dev/tty", O_WRONLY);
    if (!tty_fd)
        return;

    if (tty_fd != 1)
        dup2(tty_fd, 1);

    ft_printf("\n--- MALLOC MEMORY DUMP ---\n");
    unlock_mutex();
    show_malloc_mem();
    close(1);
}

static bool is_in_zone(void* ptr, mzone_t* zone) {
    return ptr >= zone->chunks[0].addr && ptr <= zone->chunks[CHUNKS_PER_ZONE - 1].addr;
}

static mchunk_t* search_in_zone(void* ptr, mzone_t* zone) {
    for (int i = 0; i < CHUNKS_PER_ZONE; ++i) {
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
    return ptr;
}