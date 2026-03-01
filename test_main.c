#include "src/libft_malloc.h"
#include "printf/headers/ft_printf.h"

int main (void) {
    // char* tiny;
    // for (int i = 0; i < 350; ++i) {
    //     tiny = malloc(500);
    //     if (!tiny)
    //         ft_printf("tiny malloc failed\n");
    // }

    // char* small;
    // for (int i = 0; i < 350; ++i) {
    //     small = malloc(4000);
    //     if (!small)
    //         ft_printf("small malloc failed\n");
    // }

    // char* large;
    // for (int i = 0; i < 350; ++i) {
    //     large = malloc(10000);
    //     if (!large)
    //         ft_printf("large malloc failed\n");
    // }

    char* ptr = NULL;
    for (size_t i = 0; i < 1000000; ++i) {
        ptr = malloc(50);
        ptr[0] = '1';
        ptr[49] = '2';
    }

    //show_malloc_mem();
}