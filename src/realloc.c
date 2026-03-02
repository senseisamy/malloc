#include "libft_malloc_internal.h"

void* realloc(void* ptr, size_t size) {
    if (!ptr)
        return malloc(size);

    lock_mutex();
    //ft_printf("call to realloc with ptr: %p size: %u\n", ptr, size);
    if (!mmanager.is_initialized)
        return unlock_mutex_and_return(NULL);

    mchunk_t* chunk = find_chunk(ptr);
    if (chunk) {
        unlock_mutex();
        char* new_ptr = malloc(size);
        if (!new_ptr)
            return NULL;
        lock_mutex();
        for (size_t i = 0; i < chunk->size && i < size; ++i)
            new_ptr[i] = ((char*)chunk->addr)[i];
        chunk->in_use = false;
        chunk->size = 0;
        return unlock_mutex_and_return((void*)new_ptr);
    }
    mzone_no_chunk_t* large_zone = find_large_zone(ptr);
    if (large_zone) {
        unlock_mutex();
        char* new_ptr = malloc(size);
        if (!new_ptr)
            return NULL;
        lock_mutex();
        for (size_t i = 0; i < large_zone->size && i < size; ++i)
            new_ptr[i] = ((char*)large_zone->addr)[i];
        unlock_mutex();
        free(ptr);
        return (void*)new_ptr;
    }
    ft_printf("PTR NOT FOUND\n");
    show_malloc_mem();
    return unlock_mutex_and_return(NULL);
}