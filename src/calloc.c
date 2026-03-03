#include "libft_malloc_internal.h"

void* calloc(size_t count, size_t size) {
    lock_mutex();

    void* ret = NULL;
    if (!mmanager.is_initialized)
        if (!initialize_mmanager())
            goto ret;

    if (mmanager.debug_properties.enable_logs)
        ft_printf("call to %scalloc(%u, %u)%s, ", ITALIC, count, size, RESET);
    
    size_t total_size;
    if (__builtin_mul_overflow(count, size, &total_size))
        goto ret;

    ret = malloc_internal(total_size);
    for (size_t i = 0; i < total_size; ++i)
        ((char*)ret)[i] = 0;

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
