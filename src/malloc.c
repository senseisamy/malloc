#include "libft_malloc_internal.h"

mchunk_t* get_first_available_chunk(mzone_t* zone, size_t chunk_size) {
    while (zone) {
        if (!zone->is_full) {
            for (int i = 0; i < MALLOC_PER_ZONE; ++i) {
                mchunk_t* chunk = &zone->chunks[i];
                if (!chunk->in_use)
                    return chunk;
            }
            zone->is_full = true;
        }
        if (zone->next == NULL)
            zone->next = malloc_new_zone(chunk_size);
        zone = zone->next;
    }
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
        if (!(*start_zone)->next)
            return NULL;
        return (*start_zone)->next;
    }
}

void* malloc_internal(size_t size) {
    if (size == 0)
        return NULL;
    if (size <= (size_t)SMALL_MALLOC_SIZE) { // tiny or small malloc
        mchunk_t* chunk = NULL;
        if (size <= (size_t)TINY_MALLOC_SIZE)
            chunk = get_first_available_chunk(mmanager.tiny_malloc_zones, TINY_MALLOC_SIZE);
        else
            chunk = get_first_available_chunk(mmanager.small_malloc_zones, SMALL_MALLOC_SIZE);

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
        return zone->addr;
    }
}

void* malloc(size_t size) {
    lock_mutex();

    void* ret = NULL;
    if (!mmanager.is_initialized)
        if (!initialize_mmanager())
            goto ret;

    if (mmanager.debug_properties.enable_logs)
        ft_printf("call to %smalloc(%u)%s, ", ITALIC, size, RESET);
    
    ret = malloc_internal(size);

    ret:
        if (mmanager.debug_properties.enable_logs) {
            if (!mmanager.is_initialized)
                ft_printf("%sFailed to initialize mmanager%s\n", RED, RESET);
            else
                ft_printf("returning %s%p%s\n", (ret ? GREEN : RED), ret, RESET);
        }
        unlock_mutex();
        return ret;
}