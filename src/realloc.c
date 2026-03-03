#include "libft_malloc_internal.h"

void* realloc(void* ptr, size_t size) {
    lock_mutex();

    void* ret = NULL;
    if (!mmanager.is_initialized)
        if (!initialize_mmanager())
            goto ret;

    if (mmanager.debug_properties.enable_logs)
        ft_printf("call to %srealloc(%p, %u)%s, ", ITALIC, ptr, size, RESET);

    if (!ptr) {
        ret = malloc_internal(size);
        goto ret;
    }

    mchunk_t* chunk = find_chunk(ptr);
    if (chunk) {
        ret = malloc_internal(size);
        if (!ret)
            goto ret;
        for (size_t i = 0; i < chunk->size && i < size; ++i)
            ((char*)ret)[i] = ((char*)chunk->addr)[i];
        chunk->in_use = false;
        chunk->size = 0;
        goto ret;
    }
    mzone_no_chunk_t* large_zone = find_large_zone(ptr);
    if (large_zone) {
        ret = malloc_internal(size);
        if (!ret)
            return NULL;
        for (size_t i = 0; i < large_zone->size && i < size; ++i)
            ((char*)ret)[i] = ((char*)large_zone->addr)[i];
        free_internal(ptr);
        goto ret;
    }

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
