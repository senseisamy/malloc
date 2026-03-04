#include "libft_malloc_internal.h"

void* realloc(void* ptr, size_t size) {
    lock_mutex();

    void* ret = NULL;
    if (!mmanager.is_initialized)
        if (!initialize_mmanager())
            goto ret;

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
            goto ret;
        for (size_t i = 0; i < large_zone->size && i < size; ++i)
            ((char*)ret)[i] = ((char*)large_zone->addr)[i];
        free_internal(ptr);
        goto ret;
    }

    ret:
        switch (mmanager.debug.log_level) {
            case DEBUG:
                if (ret)
                    ft_printf("call to %srealloc(%p, %u)%s, returning %s%p%s\n", ITALIC, ptr, size, RESET, GREEN, ret, RESET);
            case WARNING:
                if (!ret)
                    ft_printf("call to %srealloc(%p, %u)%s, returning %s%p%s\n", ITALIC, ptr, size, RESET, RED, ret, RESET);
            case ERROR:
                if (!mmanager.is_initialized)
                    ft_printf("%sFailed to initialize mmanager%s\n", RED, RESET);
            case NONE:
        }
        unlock_mutex();
        return ret;
}
