# 42 Malloc Project

A custom thread-safe C memory allocator that implements the malloc library functions.  
It creates a `libft_malloc.so` which exposes the following functions:
```c
// standard functions
void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void* calloc(size_t count, size_t size);
void  free(void* ptr);

// debug function defined in <malloc.h>, implementing it makes the library even more compatible
size_t malloc_usable_size(void *ptr);

// debug function that prints the state of the internal alloctor
void  show_malloc_mem();
```

# Build and Use

## Build

To build, simply run `make` and it will generate the library at the root of the directory.

## Use

To use this library instead of the libc malloc, it has to be loaded before. This can be done on linux with the `LD_PRELOAD=` environment variable like this:
```bash
LD_PRELOAD=./libft_malloc.so program
```

## Debug environment variable

You can set those environment variable while using the library:
- `FT_MALLOC_LOGS=` : 
  - `DEBUG` : prints every call to the standard functions with their parameters and return, as well as internal lib errors
  - `WARNING` : same as DEBUG but only when the functions return NULL or fails
  - `ERROR` : only prints internal lib errors
- `FT_MALLOC_DUMP_MEM_ON_EXIT=1` : calls `show_malloc_mem()` at the end of the program

# Implementation

The way to ask the OS for memory is through the `mmap` system call (which is slow, we can't call mmap every time malloc is called). To be efficient we then have to pre-allocate memory for future `malloc` calls. This pre-allocation is done **zones** which each contain multiple **chunks**. These chunks are the memory returned by malloc. There are two types of pre-allocated zones, **tiny** ones and **small** ones, which respectively contains chunks of size `TINY_MALLOC_SIZE` and `SMALL_MALLOC_SIZE` *(the values are defined in `libft_malloc_internal.h`)*. If malloc asks for an allocation bigger than that we allocate with `mmap` directly the size asked.