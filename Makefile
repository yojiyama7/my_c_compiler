CC      := cc
CFLAGS  := -std=c11 -Wall -Wextra -Werror -Wimplicit-fallthrough -g -static
LDFLAGS := 
NAME := mycc
SRCS := main.c \
        parse.c \
        codegen.c
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

.PHONY: test
test: $(NAME)
	./test.sh

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)

$(OBJS): mycc.h

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<