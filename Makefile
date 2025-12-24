CC := cc
CFLAGS := -std=c11 -Wall -Wextra -Werror -g -static
NAME := mycc
SRCS := main.c
OBJS := $(SRCS:.c=.o)

.PHONY: all
all: $(NAME)

.PHONY: run
run: $(NAME)
	./$(NAME)

.PHONY: clean
clean:
	rm -f $(NAME) *.o *~ tmp*

.PHONY: re
re:
	$(MAKE) clean
	$(MAKE) all

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<