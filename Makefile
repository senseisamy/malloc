CC		= cc
CFLAGS	= -Wall -Wextra -Werror -g3 -c -fPIC

SRCS	= $(wildcard src/*.c)
		
OBJS	= $(patsubst src/%.c, $(BIN_DIR)/%.o, $(SRCS))

BIN_DIR = bin

BIN_TEST = malloc_test

ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME	= libft_malloc_$(HOSTTYPE).so

all: $(NAME)

$(BIN_DIR):
	@mkdir -p bin

$(BIN_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(NAME): ${BIN_DIR} $(OBJS)
	make -C printf
	$(CC) -shared $(OBJS) -o $(NAME) -L./printf -lftprintf
	
clean:
	make -C printf clean
	rm -rf $(BIN_DIR)

fclean: clean
	make -C printf fclean
	rm -rf $(BIN_DIR)
	rm -f $(NAME)
	rm -rf $(BIN_TEST)

re: fclean
	make all

test_main: $(NAME)
	$(CC) -Wall -Wextra -Werror -g3 -o $(BIN_TEST) test_main.c -L./ -lft_malloc_$(HOSTTYPE)

.PHONY: all clean fclean re