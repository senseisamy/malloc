CC		= cc
CFLAGS	= -Wall -Wextra -Werror -g3

INCLUDE	= -I includes -I libft/headers

LIB		= -L libft -lft

SRCS	= $(wildcard src/*.c)
		
OBJS	= $(patsubst src/%.c, $(BIN_DIR)/%.o, $(SRCS))

BIN_DIR = bin

ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME	= libft_malloc_$(HOSTTYPE).so

all: $(NAME)

$(BIN_DIR):
	@mkdir -p bin

$(BIN_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $(INCLUDE) -o $@ $<

$(NAME): ${BIN_DIR} $(OBJS)
	make -C libft
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(INCLUDE) $(LIB)
	
clean:
	make -C libft clean
	rm -rf $(BIN_DIR)

fclean: clean
	make -C libft fclean
	rm -rf $(BIN_DIR)

re: fclean
	make all

.PHONY: all clean fclean re