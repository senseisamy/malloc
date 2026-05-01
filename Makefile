CC		= cc
CFLAGS	= -Wall -Wextra -Werror -Wimplicit-fallthrough=0 -g3 -c -fPIC -pthread

SRCS	= $(wildcard src/*.c)
		
OBJS	= $(patsubst src/%.c, $(BIN_DIR)/%.o, $(SRCS))

INCLUDES = src/libft_malloc_internal.h

PRINTF = printf/libftprintf.a

BIN_DIR = bin

ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME	= bin/libft_malloc_$(HOSTTYPE).so

NAME_S = libft_malloc.so

all: $(NAME_S)

$(BIN_DIR):
	@mkdir -p bin

$(BIN_DIR)/%.o: src/%.c $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ $<

$(NAME): $(PRINTF) ${BIN_DIR} $(OBJS)
	$(CC) -shared $(OBJS) -o $(NAME) -L./printf -lftprintf

$(NAME_S): $(NAME)
	ln -s -f $(NAME) libft_malloc.so
	
$(PRINTF):
	make -C printf

clean:
	make -C printf clean
	rm -rf $(BIN_DIR)

fclean: clean
	make -C printf fclean
	rm -rf $(BIN_DIR)
	rm -f $(NAME)
	rm -f libft_malloc.so

re: fclean
	make all

.PHONY: all clean fclean re