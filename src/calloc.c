#include "libft_malloc_internal.h"

void* calloc(size_t count, size_t size) {
    lock_mutex();

    void* ret = NULL;
    if (!mmanager.is_initialized)
        if (!initialize_mmanager())
            goto ret;
    
    size_t total_size;
    if (__builtin_mul_overflow(count, size, &total_size))
        goto ret;

    ret = malloc_internal(total_size);
    for (size_t i = 0; i < total_size; ++i)
        ((char*)ret)[i] = 0;

    ret:
        switch (mmanager.debug.log_level) {
            case DEBUG:
                if (ret)
                    ft_printf("call to %scalloc(%u, %u)%s, returning %s%p%s\n", ITALIC, count, size, RESET, GREEN, ret, RESET);
            case WARNING:
                if (!ret)
                    ft_printf("call to %scalloc(%u, %u)%s, returning %s%p%s\n", ITALIC, count, size, RESET, RED, ret, RESET);
            case ERROR:
                if (!mmanager.is_initialized)
                    ft_printf("%sFailed to initialize mmanager%s\n", RED, RESET);
            case NONE:
        }
        unlock_mutex();
        return ret;
}
