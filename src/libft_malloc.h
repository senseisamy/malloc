#ifndef LIBFT_MALLOC
# define LIBFT_MALLOC

# include <stddef.h>
# include <unistd.h>
# include <stdbool.h>
# include <sys/mman.h>

# if defined(__linux__)
#  define PAGE_SIZE sysconf(_SC_PAGESIZE)
# elif defined(__APPLE__)
#  define PAGE_SIZE getpagesize()
# else
#  define PAGE_SIZE 4096
# endif

# define TINY_MALLOC_SIZE 512 // we have to make sure this value is a mutliple of 16 for the memory to stay aligned
# define SMALL_MALLOC_SIZE 4096 // same here
# define TINY_MALLOC_ZONE_PREALLOCATED 2
# define SMALL_MALLOC_ZONE_PREALLOCATED 2
# define MALLOC_PER_ZONE 100

typedef struct mchunk_s {
    void* addr;
    size_t size;
    bool in_use;
} mchunk_t;

typedef struct mzone_s {
    mchunk_t chunks[MALLOC_PER_ZONE];
    void *addr; // address of where the free and aligned memory starts
    bool is_full;
    struct mzone_s *next;
} mzone_t;

typedef struct mzone_no_chunk_s {
    void *addr;
    size_t size;
    struct mzone_no_chunk_s *next;
} mzone_no_chunk_t;

typedef struct mmanager_s {
    bool is_initialized;
    mzone_t* tiny_malloc_zones;
    mzone_t* small_malloc_zones;
    mzone_no_chunk_t* large_malloc_zones;
} mmanager_t;

mmanager_t mmanager; // global memory manager

bool initialize_mmanager();
mchunk_t* get_first_available_chunk(mzone_t* zone);
mzone_no_chunk_t* malloc_and_get_zone(mzone_no_chunk_t** start_zone, size_t size);

#endif