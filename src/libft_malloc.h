#ifndef LIBFT_MALLOC
# define LIBFT_MALLOC

# include <stddef.h>
# include <unistd.h>
# include "../libft/headers/libft.h"

# ifdef __linux__
#  define PAGE_SIZE sysconf(_SC_PAGESIZE)
# elifdef __APPLE__
#  define PAGE_SIZE getpagesize()
# else
#  define PAGE_SIZE 4096
# endif

# define TINY_MALLOC_SIZE PAGE_SIZE/32
# define SMALL_MALLOC_SIZE PAGE_SIZE/2
# define TINY_MALLOC_ZONE_PREALLOCATED 1
# define SMALL_MALLOC_ZONE_PREALLOCATED 1
# define MALLOC_PER_ZONE 100

typedef struct mchunk_s {
    void* addr;
    size_t size;
    bool in_use;
} mchunk_t;

typedef struct mzone_s {
    mchunk_t chunks[MALLOC_PER_ZONE];
    void *addr;
    bool is_full;
    mzone_t *next;
} mzone_t;

typedef struct mzone_no_chunk_s {
    void *addr;
    size_t size;
    bool is_allocated;
    mzone_no_chunk_t *next;
} mzone_no_chunk_t;

typedef struct mmanager_s {
    bool is_initialized;
    mzone_t* tiny_malloc_zones;
    mzone_t* small_malloc_zones;
    mzone_no_chunk_t* large_malloc_zones;
} mmanager_t;

#endif