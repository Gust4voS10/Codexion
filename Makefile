NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread

SRC_DIR		= srcs
OBJ_DIR		= objs
INC_DIR		= includes

SRCS		= main.c \
			  parsing.c \
			  init.c \
			  cleanup.c \
			  utils.c \
			  logger.c \
			  dongles.c \
			  coder.c \
			  monitor.c \
			  scheduler.c \
			  scheduler_fifo.c \
			  scheduler_edf.c

OBJS		= $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

debug:
	$(CC) $(CFLAGS) -g $(SRCS) -o $(NAME)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re