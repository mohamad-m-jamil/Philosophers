# Variables
NAME = philosophers
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pthread
SRCDIR = src
INCDIR = include
OBJDIR = obj
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Default rule
all: $(NAME)

# Linking the final executable
$(NAME): $(OBJ)
	@echo "=============================="
	@echo "🔧 Building libft... 🔧"
	@echo "=============================="
	@make -C $(LIBFTPATH)
	@echo "=============================="
	@echo "⚙️ Compiling the program... ⚙️"
	@echo "=============================="
	@$(CC) $(OBJS) $(LIBFT) $(MLXFLAGS) $(CFLAGS) -o $(NAME)
	@echo "=================================="
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

# Compiling object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# Cleaning up object files
clean:
	rm -rf $(OBJDIR)

# Cleaning everything, including the executable
fclean: clean
	rm -f $(NAME)

# Rebuild everything
re: fclean all

# Phony targets (not actual files)
.PHONY: all clean fclean re
