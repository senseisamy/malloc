NAME = libftprintf.a
SRCSDIR = srcs
OBJSDIR = objs
LIBDIR = libft
SRCS = $(addprefix $(SRCSDIR)/, ft_printf_c.c ft_printf_di.c ft_printf_p.c ft_printf_s.c ft_printf_u.c ft_printf_x.c ft_vdprintf.c ft_printf.c)
OBJS = $(addprefix $(OBJSDIR)/, $(notdir $(SRCS:.c=.o)))

CC = gcc
CFLAGS = -Wall -Wextra -Werror
AR = ar rc
RM = rm -rf
MKDIR = mkdir -p

all: $(NAME)

$(OBJSDIR):
	$(MKDIR) $(OBJSDIR)

$(OBJSDIR)/%.o : $(SRCSDIR)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@


$(NAME): $(OBJSDIR) $(OBJS)
	$(AR) $(NAME) $(OBJS)

bonus: $(NAME)

clean:
	$(RM) $(OBJSDIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
