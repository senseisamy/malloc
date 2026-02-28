CC		= cc
CFLAGS	= -Wall -Wextra -Werror -g3 -c -fPIC -pthread

SRCS	= $(wildcard src/*.c)
		
OBJS	= $(patsubst src/%.c, $(BIN_DIR)/%.o, $(SRCS))

BIN_DIR = bin

BIN_TEST = malloc_test

ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME	= bin/libft_malloc_$(HOSTTYPE).so

NAME_S = libft_malloc.so

all: $(NAME_S)

$(BIN_DIR):
	@mkdir -p bin

$(BIN_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(NAME): printf ${BIN_DIR} $(OBJS)
	$(CC) -shared $(OBJS) -o $(NAME) -L./printf -lftprintf

$(NAME_S): $(NAME)
	ln -s -f $(NAME) libft_malloc.so
	
printf:
	make -C printf

clean:
	make -C printf clean
	rm -rf $(BIN_DIR)

fclean: clean
	make -C printf fclean
	rm -rf $(BIN_DIR)
	rm -f $(NAME)
	rm -rf $(BIN_TEST)
	rm libft_malloc.so

re: fclean
	make all

$(BIN_TEST): $(NAME_S)
	$(CC) -Wall -Wextra -Werror -g3 -o $(BIN_TEST) test_main.c -L./ -lft_malloc

.PHONY: all clean fclean re printf