NAME= "Curses Bird"

CFLAGS= -g

build:	main.o
	gcc -o ${NAME} main.o -lcurses $(CFLAGS)

clean:
	rm main.o

fclean: clean
	${NAME}
