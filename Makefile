NAME        = ft_traceroute

CC          = gcc
CFLAGS      = -Wall -Wextra -Werror -Iincludes

OBJ_DIR     = obj
SRC_DIR     = srcs

SRC         = main.c \
              net.c \
              traceroute.c

OBJ         = $(SRC:%.c=$(OBJ_DIR)/%.o)

GREEN       := \033[0;32m
YELLOW      := \033[0;33m
CYAN        := \033[0;36m
MAGENTA     := \033[0;35m
BOLD        := \033[1m
RESET       := \033[0m

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "$(WHITE)"
	@echo "                            _,-}}-._"
	@echo "                           /\\   }  /\\"
	@echo "                         _|(O\\_ _/O)                  ___|_"
	@echo "                        _|/  (__''__)                 |#####|"
	@echo "                      _|\\/    WVVVVW                  |#####|"
	@echo "                     \\ _\\     \\MMMM/_             .-----.###|"
	@echo "                   _|\\_\\     _ '---; \\_           |#####|###|"
	@echo "              /\\   \\ _\\/      \\_   /   \\          |#####|###|"
	@echo "             / (    _\\/     \\   \\  |'VVV     P    |#####|###| "
	@echo "            (  '-,._\\_.(      'VVV /      (_/|\\_) |#####|###|"
	@echo "             \\         /   _) /   _)     (_/_ _\\_)|#####|###|"
	@echo "              '....--''\\__vvv)\\__vvv)_____|_| |_|_|#####|###|_____"
	@echo "$(RESET)"
	@echo "$(GREEN)$(BOLD)Build complete! ✨$(RESET)"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(CYAN)Cleaning up...$(RESET)"
	@sleep 0.3
	rm -rf $(OBJ_DIR)
	@echo "$(GREEN)Clean complete! 🧹$(RESET)"

fclean: clean
	@echo "$(CYAN)Removing executable...$(RESET)"
	@sleep 0.3
	rm -f $(NAME)
	@echo "$(GREEN)Full clean complete! 🧼$(RESET)"

re: fclean all

.PHONY: all clean fclean re