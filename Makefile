NAME= "Curses Bird"

include config.mk

CFLAGS += -std=gnu99

LDFLAGS += $(CURSES_LIB)

SRC =	main.c

OBJ =	$(SRC:.c=.o)

build: $(OBJ)
	$(CC) -o $(NAME) main.o $(CFLAGS) $(LDFLAGS)

clean: $(OBJ)
	rm main.o

fclean: clean
	rm $(NAME)
