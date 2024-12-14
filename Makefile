NAME	= philo
CC		= gcc
CFLAGS	= -Werror -Wall -Wextra -pthread

# Run 'make re MODE=pretty' for color formatting.
# Run 'make re MODE=debug' for debug formatting.
MODE	= none
ifeq ($(MODE), pretty)
	CFLAGS	+= -D DEBUG_FORMATTING=1
endif
ifeq ($(MODE), debug)
	CFLAGS	+= -D DEBUG_FORMATTING=1 -fsanitize=thread -g
endif

SRC_PATH = sources/
OBJ_PATH = objects/

SRC		=	main.c \
			utils2.c \
			monitor_simulation.c \
			time.c \
			philo.c \
			init.c \
			output.c \
			utils.c
SRCS	= $(addprefix $(SRC_PATH), $(SRC))
OBJ		= $(SRC:.c=.o)
OBJS	= $(addprefix $(OBJ_PATH), $(OBJ))

INC		= -I ./includes/

all: $(NAME)

$(OBJ_PATH)%.o: $(SRC_PATH)%.c
	@mkdir -p $(OBJ_PATH)
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@
	@echo "✅ Build complete! ✅"
	@echo "=================================="
	@echo "\033[31m ███▄ ▄███▓    ▄▄▄██▀▀▀    ▄▄▄          ███▄ ▄███▓    ██▓    ██▓    "
	@echo "▓██▒▀█▀ ██▒      ▒██      ▒████▄       ▓██▒▀█▀ ██▒   ▓██▒   ▓██▒    "
	@echo "▓██    ▓██░      ░██      ▒██  ▀█▄     ▓██    ▓██░   ▒██▒   ▒██░    "
	@echo "▒██    ▒██    ▓██▄██▓     ░██▄▄▄▄██    ▒██    ▒██    ░██░   ▒██░    "
	@echo "▒██▒   ░██▒    ▓███▒       ▓█   ▓██▒   ▒██▒   ░██▒   ░██░   ░██████▒"
	@echo "░ ▒░   ░  ░    ▒▓▒▒░       ▒▒   ▓▒█░   ░ ▒░   ░  ░   ░▓     ░ ▒░▓  ░"
	@echo "░  ░      ░    ▒ ░▒░        ▒   ▒▒ ░   ░  ░      ░    ▒ ░   ░ ░ ▒  ░"
	@echo "░      ░       ░ ░ ░        ░   ▒      ░      ░       ▒ ░     ░ ░   "
	@echo "       ░       ░   ░            ░  ░          ░       ░         ░  ░"
	@echo "\033[0m"  # Reset color
	@echo "\033[0m"  # Reset color
	@echo "Well done! 🎯"
clean:
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all re clean fclean