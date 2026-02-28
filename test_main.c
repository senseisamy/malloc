#include "src/libft_malloc.h"
#include "printf/headers/ft_printf.h"

int main (void) {
    char* tiny = malloc(500);
    tiny[0] = 'c';
    tiny[400] = 'a';

    char* small = malloc(3000);
    small[0] = 'a';
    small[2067] = 'b';

    char* large = malloc(1000000);
    large[100000] = 'a';
    char* large2 = malloc(363667);
    large2[67] = 'b';
    large2 = realloc(large2, 676767);
    ft_printf("large2[67]=%c\n", large2[67]);

    show_malloc_mem();

    ft_printf("\nfreeing memory\n\n");
    free(tiny);
    free(small);
    free(large);

    show_malloc_mem();
}