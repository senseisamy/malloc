#ifndef LIBFT_MALLOC
# define LIBFT_MALLOC

# include <stddef.h>
# include <unistd.h>
# include <stdbool.h>
# include <sys/mman.h>
# include <pthread.h>
# include "../printf/headers/ft_printf.h"

# if defined(__linux__)
#  define PAGE_SIZE sysconf(_SC_PAGESIZE)
# elif defined(__APPLE__)
#  define PAGE_SIZE getpagesize()
# else
#  define PAGE_SIZE 4096
# endif

# define TINY_MALLOC_SIZE 512 // we have to make sure this value is a mutliple of 16 for the memory to stay aligned
# define SMALL_MALLOC_SIZE 4096 // same here
# define TINY_MALLOC_ZONE_PREALLOCATED 5 // number of tiny zones preallocated at the start
# define SMALL_MALLOC_ZONE_PREALLOCATED 5 // number of small zones preallocated at the start
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

extern mmanager_t mmanager; // global memory manager
extern pthread_mutex_t mutex_malloc; // global mutex for thread safety

bool initialize_mmanager();
void show_malloc_mem();
size_t round_up_to(size_t a, size_t b);
mchunk_t* find_chunk(void* ptr);
mzone_no_chunk_t* find_large_zone(void* ptr);
void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);

void lock_mutex();
void unlock_mutex();
void* unlock_mutex_and_return(void* ptr);

#endif